#include "connection_uec.h"
//#include "daihinmin.h"
#include "config.h"
#include "position.h"
#include <iostream>

using namespace std;

#define CONNECTION_DEBUG

// 通信、及びゲームの実行。プロトコルは http://www.tnlab.inf.uec.ac.jp/daihinmin/2016/document_protocol.html を参照
void ConnectionUEC::execute(char server_name_in[256], uint16_t port_in, char user_name_in[15]){
  // パラメタ指定
  strcpy(server_name, server_name_in);
  port = port_in;
  strcpy(user_name, user_name_in);

  // 通信1（全プレイヤー）自分のID受け取る
  myid = entryToGame();
  game_count = 1;
  // 以下、ゲームが終わるまでループ
  while(1){
    cout<<"game "<<game_count<<endl;
    game_count++;
    // 通信2（全プレイヤー）最初の手札を受け取る
    receiveTable();
    agent->initgame(recv_table);
    cout<<"hand dist done"<<endl;
    // escape
      
    // 通信3-1（自分が大富豪or富豪の時）カード交換を行う
    //カード交換時フラグをチェック recv_table[5][0]の数が交換枚数
    if(recv_table[5][0]== 0){
      cout << "no change phase (i am heimin or hinmin or daihinmin)"<<endl;
    }else{ 
      if(recv_table[5][1] > 0 && recv_table[5][1]<100){
	// 何らかの方法でカードを決める
	int cards_after_change[8][15];
	// 通信3-2 交換結果を返す
	// cards_after_change[0-3][1-13]でそれぞれ、スート、数字を示している（配列4 in 公式）
	// 返すカードだけ1として他は0にすること
	agent->send_change(recv_table, cards_after_change);
	sendTable(cards_after_change);
      }
    }
    
    while(1){
      // 以下、4人が上がるまで戦い続ける...
      // 盤面の代入
      receiveTable();
      agent->receive_board(recv_table);
      cout<<"receive board"<<endl;
      // 自分の手番であるなら
      if(recv_table[5][2]!=0){
	cout<<"myturn"<<endl;
	int cards_to_push[8][15];
	agent->search(cards_to_push);
	sendTable(cards_to_push);
	// 送信の可否を受け取る。仮にrejectされたとして、それをどうやって扱えばいいんだ？
	// 整数値を受け取る(手の可否)
	int ok = receiveInt();
	cout<<"submit status "<<ok<<endl;
	if(ok == 8){
	  bool show = false; //passをrejectする変な仕様なので。
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
	// 各プレイヤーの指し手
	receiveTable();
	agent->receive_ply(recv_table);
      }else{

	// 各プレイヤーの指し手
	receiveTable();
	agent->receive_ply(recv_table);
      }

      //
      // ゲーム終了ならループを抜ける
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
    
  }//全ゲームが終わるまでの繰り返しここまで

}


void ConnectionUEC::setAgent(DagentUEC *inagent){
  agent = inagent;
}


void ConnectionUEC::checkArg(int argc,char* argv[]){
  /*
    渡されたコマンドラインから与えられた引数の情報を解析し、必要に応じて
    サーバアドレス、ポート番号、クライアント名を変更する。
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


//ソケットの設定・接続を行う 成功時0、失敗時-1を返す
int ConnectionUEC::openSocket(const char addr[], uint16_t port_num){
  //ソケットの生成
  if ((g_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    return(-1);
  }
  
  /* ポートとアドレスの設定 */
  bzero((char*)&g_client_addr,sizeof(g_client_addr));
  g_client_addr.sin_family = PF_INET;
  g_client_addr.sin_port = htons(port_num);
  g_client_addr.sin_addr.s_addr = inet_addr(addr);

  //IPアドレスで指定されていないとき、ホスト名の解決を試みる
  if (g_client_addr.sin_addr.s_addr == 0xffffffff) {
    struct hostent *host;
    host = gethostbyname(addr);
    if (host == NULL) {
      printf("failed to gethostbyname() : %s.\n",addr);
      return -1;//ホスト名解決に失敗したとき、-1を返す
    }
    g_client_addr.sin_addr.s_addr=
      *(unsigned int *)host->h_addr_list[0];
  }

  /* サーバにコネクトする */
  if (connect(g_sockfd,(struct sockaddr *)&g_client_addr, sizeof(g_client_addr)) == 0){
    return 0;
  }
  return -1;
}

//クライアントの情報を送信
int ConnectionUEC::sendProfile(const char user[15]){
  int profile[8][15];
  int i;

  bzero((char *) &profile, sizeof(profile));        //テーブルを0でうめる
  profile[0][0]=PROTOCOL_VERSION;                   //2007年度版を宣言
  for(i=0;i<15;i++) profile[1][i]=(int)user[i];     //制作者名をテーブルに格納

  //送信
  if(sendTable(profile)==-1){                       //失敗したらエラーを出力し停止
    printf("sending profile table was failed.\n");
    //return -1;
    exit (1);
  }

  return 0;
}

//サーバにテーブル情報を投げる関数。成功なら0　失敗時-1を返す
int ConnectionUEC::sendTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  int i,j;
  //全てのテーブルの要素をホストオーダーからネットワークオーダーに変換
  for(i=0;i<8;i++)
    for(j=0;j<15;j++)
      net_table[i][j]=htonl(table_val[i][j]);
  //変換したテーブルを送信
  if((g_buf_len = write(g_sockfd, net_table, 480))> 0){
    return (0);
  }
  else{
    return (-1);
  }
}


//hostに接続し ゲームに参加する プレーヤー番号を返す
int ConnectionUEC::entryToGame(void){
  int my_playernum;  //プレイヤー番号を記憶する
  //サーバに対してソケットを用意し、connectする
  cout<<"entry to game"<<endl;
  if((openSocket(server_name,port))== -1){
    printf("failed to open socket to server[%s:%d].\n",server_name,port);
    //return -1;
    exit (1);
  }
  cout<<"send profile"<<endl;
  sendProfile(user_name);     //クライアントの情報を送信
  cout<<"send profile done"<<endl;
  
  //自身のプレイヤー番号をサーバからもらう
  cout<<"receive plnumber"<<endl;
  if(read(g_sockfd, &my_playernum, sizeof(my_playernum)) > 0){
    my_playernum=ntohl(my_playernum);
  }
  else{
    printf("failed to get player number.\n");
    exit (1);
  }
  cout<<"done"<<endl;

  return my_playernum; //自身のプレイヤー番号を返す。
}


//サーバからテーブル情報を受け取り、成功なら0失敗なら-1を返す
int ConnectionUEC::receiveTable(){
  
  uint32_t net_table[8][15];
  if ((g_buf_len = read(g_sockfd,net_table, 480)) > 0){
    int i,j;
    //全てのテーブルの要素をネットワークオーダーからホストオーダーに変換
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
    //受信失敗時 メッセージを表示し停止
    cout << "failed to check if the game was finished" << endl;
    return -1;
    
  }else{
    //受信成功時 値のバイトオーダーを直す
    signal=ntohl(signal);
  }
  return signal;
}

//ソケットのcloseを行う 成功した場合は 0 を返す。エラーが発生した場合は -1 を返す
int ConnectionUEC::closeSocket(){
  return close(g_sockfd);
}


// escape
