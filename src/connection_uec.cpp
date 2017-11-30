#include "connection_uec.h"
#include "daihinmin.h"
#include "config.h"
#include "position.h"
#include <iostream>

using namespace std;

#define CONNECTION_DEBUG

// �ʐM�A�y�уQ�[���̎��s�B�v���g�R���� http://www.tnlab.inf.uec.ac.jp/daihinmin/2016/document_protocol.html ���Q��
void ConnectionUEC::execute(char server_name_in[256], uint16_t port_in, char user_name_in[15]){
  // �p�����^�w��
  strcpy(server_name, server_name_in);
  port = port_in;
  strcpy(user_name, user_name_in);

  // �ʐM1�i�S�v���C���[�j������ID�󂯎��
  myid = entryToGame();
  game_count = 1;
  // �ȉ��A�Q�[�����I���܂Ń��[�v
  while(1){
    cout<<"game "<<game_count<<endl;
    game_count++;
    // �ʐM2�i�S�v���C���[�j�ŏ��̎�D���󂯎��
    receiveTable();
    agent->initgame(recv_table);
    cout<<"hand dist done"<<endl;
    // escape
      
    // �ʐM3-1�i��������x��or�x���̎��j�J�[�h�������s��
    //�J�[�h�������t���O���`�F�b�N recv_table[5][0]�̐�����������
    if(recv_table[5][0]== 0){
      cout << "no change phase (i am heimin or hinmin or daihinmin)"<<endl;
    }else{ 
      if(recv_table[5][1] > 0 && recv_table[5][1]<100){
	// ���炩�̕��@�ŃJ�[�h�����߂�
	int cards_after_change[8][15];
	// �ʐM3-2 �������ʂ�Ԃ�
	// cards_after_change[0-3][1-13]�ł��ꂼ��A�X�[�g�A�����������Ă���i�z��4 in �����j
	// �Ԃ��J�[�h����1�Ƃ��đ���0�ɂ��邱��
	agent->send_change(recv_table, cards_after_change);
	sendTable(cards_after_change);
      }
    }
    
    while(1){
      // �ȉ��A4�l���オ��܂Ő킢������...
      // �Ֆʂ̑��
      receiveTable();
      agent->receive_board(recv_table);
      cout<<"receive board"<<endl;
      // �����̎�Ԃł���Ȃ�
      if(recv_table[5][2]!=0){
	cout<<"myturn"<<endl;
	int cards_to_push[8][15];
	agent->search(cards_to_push);
	sendTable(cards_to_push);
	// ���M�̉ۂ��󂯎��B����reject���ꂽ�Ƃ��āA������ǂ�����Ĉ����΂����񂾁H
	// �����l���󂯎��(��̉�)
	int ok = receiveInt();
	cout<<"submit status "<<ok<<endl;
	if(ok == 8){
	  bool show = false; //pass��reject����ςȎd�l�Ȃ̂ŁB
	  for(int i=0;i<8;++i){
	    for(int j=0;j<15;++j){
	      if(cards_to_push[i][j]!=0){
		show = true;
	      }
	    }
	  }
	  if(show){
	    cout<<"ply rejected!!"<<endl;
	    for(int i=0;i<8;++i){
	      for(int j=0;j<15;++j){
		cout<<cards_to_push[i][j]<<" ";
	      }
	      cout<<endl;
	    }
	  }
	}
	// �e�v���C���[�̎w����
	receiveTable();
	agent->receive_ply(recv_table);
      }else{

	// �e�v���C���[�̎w����
	receiveTable();
	agent->receive_ply(recv_table);
      }

      //
      // �Q�[���I���Ȃ烋�[�v�𔲂���
      int gameend = receiveInt();
      cout<<"gameend status "<< gameend<<endl;
      if(gameend == 1){break;}
      if(gameend == 2){
	if(closeSocket()!=0){
	  cout << "failed to close socket"<<endl;
	}	
	return;
      }
    } // game loop
    
  }//�S�Q�[�����I���܂ł̌J��Ԃ������܂�

}


void ConnectionUEC::setAgent(DagentUEC *inagent){
  agent = inagent;
}


void ConnectionUEC::checkArg(int argc,char* argv[]){
  /*
    �n���ꂽ�R�}���h���C������^����ꂽ�����̏�����͂��A�K�v�ɉ�����
    �T�[�o�A�h���X�A�|�[�g�ԍ��A�N���C�A���g����ύX����B
  */
  const char Option[]="[-h server_adress] [-p port] [-n user_name]";
  int        arg_count=1;

  while(arg_count<argc){
    if( strcmp(argv[arg_count],"--help")==0){
      printf("usage : %s %s\n",argv[0],Option);
      exit(0);
    }else if (strcmp(argv[arg_count],"-h")==0){
      arg_count++;
      if (arg_count<argc){
	strcpy(server_name,argv[arg_count]);
      }else{
	printf("%s -h hostname\n",argv[0]);
	exit(1);
      }
    }else if ((strcmp(argv[arg_count],"-p"))==0){
      arg_count++;
      if (arg_count<argc){
	port = (uint16_t)atoi(argv[arg_count]);
      }else{
	printf("%s -p port\n",argv[0]);
	exit(1);
      }
    }else if ((strcmp(argv[arg_count],"-n"))==0){
      arg_count++;
      if (arg_count<argc){
	strcpy(user_name ,argv[arg_count]);
      }else{
	printf("%s -n user_name\n",argv[0]);
	exit(1);
      }
    }else{
      printf("%s : unknown option : %s \n",argv[0],argv[arg_count]);
      printf("usage : %s %s\n",argv[0],Option);
      exit(1);
    }
    arg_count++;
  }
}


//�\�P�b�g�̐ݒ�E�ڑ����s�� ������0�A���s��-1��Ԃ�
int ConnectionUEC::openSocket(const char addr[], uint16_t port_num){
  //�\�P�b�g�̐���
  if ((g_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    return(-1);
  }
  
  /* �|�[�g�ƃA�h���X�̐ݒ� */
  bzero((char*)&g_client_addr,sizeof(g_client_addr));
  g_client_addr.sin_family = PF_INET;
  g_client_addr.sin_port = htons(port_num);
  g_client_addr.sin_addr.s_addr = inet_addr(addr);

  //IP�A�h���X�Ŏw�肳��Ă��Ȃ��Ƃ��A�z�X�g���̉��������݂�
  if (g_client_addr.sin_addr.s_addr == 0xffffffff) {
    struct hostent *host;
    host = gethostbyname(addr);
    if (host == NULL) {
      printf("failed to gethostbyname() : %s.\n",addr);
      return -1;//�z�X�g�������Ɏ��s�����Ƃ��A-1��Ԃ�
    }
    g_client_addr.sin_addr.s_addr=
      *(unsigned int *)host->h_addr_list[0];
  }

  /* �T�[�o�ɃR�l�N�g���� */
  if (connect(g_sockfd,(struct sockaddr *)&g_client_addr, sizeof(g_client_addr)) == 0){
    return 0;
  }
  return -1;
}

//�N���C�A���g�̏��𑗐M
int ConnectionUEC::sendProfile(const char user[15]){
  int profile[8][15];
  int i;

  bzero((char *) &profile, sizeof(profile));        //�e�[�u����0�ł��߂�
  profile[0][0]=PROTOCOL_VERSION;                   //2007�N�x�ł�錾
  for(i=0;i<15;i++) profile[1][i]=(int)user[i];     //����Җ����e�[�u���Ɋi�[

  //���M
  if(sendTable(profile)==-1){                       //���s������G���[���o�͂���~
    printf("sending profile table was failed.\n");
    //return -1;
    exit (1);
  }

  return 0;
}

//�T�[�o�Ƀe�[�u�����𓊂���֐��B�����Ȃ�0�@���s��-1��Ԃ�
int ConnectionUEC::sendTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  int i,j;
  //�S�Ẵe�[�u���̗v�f���z�X�g�I�[�_�[����l�b�g���[�N�I�[�_�[�ɕϊ�
  for(i=0;i<8;i++)
    for(j=0;j<15;j++)
      net_table[i][j]=htonl(table_val[i][j]);
  //�ϊ������e�[�u���𑗐M
  if((g_buf_len = write(g_sockfd, net_table, 480))> 0){
    return (0);
  }
  else{
    return (-1);
  }
}


//host�ɐڑ��� �Q�[���ɎQ������ �v���[���[�ԍ���Ԃ�
int ConnectionUEC::entryToGame(void){
  int my_playernum;  //�v���C���[�ԍ����L������
  //�T�[�o�ɑ΂��ă\�P�b�g��p�ӂ��Aconnect����
  cout<<"entry to game"<<endl;
  if((openSocket(server_name,port))== -1){
    printf("failed to open socket to server[%s:%d].\n",server_name,port);
    //return -1;
    exit (1);
  }
  cout<<"send profile"<<endl;
  sendProfile(user_name);     //�N���C�A���g�̏��𑗐M
  cout<<"send profile done"<<endl;
  
  //���g�̃v���C���[�ԍ����T�[�o������炤
  cout<<"receive plnumber"<<endl;
  if(read(g_sockfd, &my_playernum, sizeof(my_playernum)) > 0){
    my_playernum=ntohl(my_playernum);
  }
  else{
    printf("failed to get player number.\n");
    exit (1);
  }
  cout<<"done"<<endl;

  return my_playernum; //���g�̃v���C���[�ԍ���Ԃ��B
}


//�T�[�o����e�[�u�������󂯎��A�����Ȃ�0���s�Ȃ�-1��Ԃ�
int ConnectionUEC::receiveTable(){
  
  uint32_t net_table[8][15];
  if ((g_buf_len = read(g_sockfd,net_table, 480)) > 0){
    int i,j;
    //�S�Ẵe�[�u���̗v�f���l�b�g���[�N�I�[�_�[����z�X�g�I�[�_�[�ɕϊ�
    for(i=0;i<8;i++){
      for(j=0;j<15;j++){
	recv_table[i][j]=ntohl(net_table[i][j]);
      }
    }
    return 0;
  }
  else{
    cout<<"failed to receive data";
    return(-1);
  }
}


int ConnectionUEC::receiveInt(){
  int signal;
 
  if ((read(g_sockfd, &signal, sizeof(signal))) < 0 ){
    //��M���s�� ���b�Z�[�W��\������~
    cout << "failed to check if the game was finished" << endl;
    return -1;
    
  }else{
    //��M������ �l�̃o�C�g�I�[�_�[�𒼂�
    signal=ntohl(signal);
  }
  return signal;
}

//�\�P�b�g��close���s�� ���������ꍇ�� 0 ��Ԃ��B�G���[�����������ꍇ�� -1 ��Ԃ�
int ConnectionUEC::closeSocket(){
  return close(g_sockfd);
}


// escape