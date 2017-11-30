#include "agent_kowloon_uec.h"
#include "config.h"
#include "changer_uecdef.h"
#include "bithand.h"
#include "translation.h"

DagentUEC_kowloon kowloon;

#define CONNECTION_DEBUG

void state_type::initgame(){
  eply = 0;
  passnum = 0;
  flush();
}

void state_type::flush(){
  qty=0;
  ord=0;
  lock=0;
  passnum = 0;
  for(int i=0;i<5;i++)suit[i]=0;
}

void state_type::getState(int cards[8][15]){
  /*//
    カードテーブルから得られる情報を読み込む
    引数は手札のカードテーブル
    情報は広域変数stateに格納される
  */
  int i;
  //状態
  if(cards[5][4]>0) onset=1; //場にカードがないとき 1
  else              onset=0;
  if(cards[5][6]>0) rev=1;   //革命状態の時 1 
  else              rev=0;
  if(cards[5][5]>0) b11=1;   //11バック時 1 未使用
  else              b11=0;
  if(cards[5][7]>0) lock=1;  //しばり時 1
  else              lock=0;

  if(onset==1){   //新たな場のとき札の情報をリセット
    flush();
  }
  
  for(i=0;i<5;i++) player_qty[i]=cards[6][i];   //手持ちのカード
  for(i=0;i<5;i++) player_rank[i]=cards[6][5+i];//各プレーヤのランク
  for(i=0;i<5;i++) seat[i]=cards[6][10+i];      //誰がどのシートに座っているか
                                                      //シートiにプレーヤ SEAT[I]が座っている

  if(cards[4][1]==2) joker=1;     //Jokerがある時 1
  else               joker=0;

  
}

void state_type::getField(int cards[8][15]){
  /*
    場に出たカードの情報を得る。
    引数は場に出たカードのテーブル
    情報は広域変数stateに格納される
  */
  uint64_t befep = eply;
  eply = 0;
  int i,j,count=0;
  i=j=0;
  
  //カードのある位置を探す
  while(j<15&&cards[i][j]==0){
    suit[i]=0;
    i++;
    if(i==4){
      j++;
      i=0;
    }
  }
  //階段が否か
  if(j<14){
    if(cards[i][j+1]>0) sequence=1;
    else sequence=0;
  }
  //枚数を数える また強さを調べる
  if(sequence==0){
    //枚数組
    for(;i<5;i++){
      if(cards[i][j]>0){
	count++;
	suit[i]=1;
	if(cards[i][j] == 1 && i<4){
	  eply = eply | (CARDS_HORIZON << (j*4+i));
	}else if(cards[i][j] == 2){
	  eply = eply | CARD_JOKER;
	}
      }else{
	suit[i]=0;
      }
    }
    if(j==0||j==14){
      if(rev==0){
	ord=13;
      }else{
	ord=1;
      }
    }else{
      ord=j;
    }
  }else{
    //階段
    while(j+count<15 && cards[i][j+count]>0){
      if(cards[i][j+count] == 1){
	eply = eply | (CARDS_HORIZON << ((j+count)*4+i));
      }else if(cards[i][j+count] == 2){
	eply = eply | CARD_JOKER;
      }
      count++;
    }
    ord = j;
    suit[i]=1;
  }
  //枚数を記憶
  qty=count;
 
  if(qty>0){ //枚数が0より大きいとき 新しい場のフラグを0にする
    onset=0;
  }
  if(befep != eply){ 
    passnum +=1;
  }
}

void state_type::state2pos(Position &pos){
  pos.board.rank = ord;
  if(onset){
    pos.board.type = PASS;
  }else if(sequence){
    pos.board.type = handtype(int(STAIR3) + min(3,qty - 3));
  }else{
    pos.board.type = handtype(int(PAIR1) + (qty-1));
  }
  if(lock == 0){
    pos.board.suitlock = 0;
  }else{
    for(int i=0;i<4;++i){
      if(suit[i]){
	pos.board.suitlock = pos.board.suitlock | (1<<i);
      }
    }
  }
  pos.board.cards = eply;
  pos.kakumei = (rev == 0 ? false : true);
  pos.setEply(eply);
  
}

void state_type::showState(){
  /*引数で渡された状態stateの内容を表示する*/
  int i;
  printf("state rev   : %d\n",rev);
  printf("state lock  : %d\n",lock);
  printf("state joker : %d\n",joker);
  
  printf("state qty   : %d\n",qty);
  printf("state ord   : %d\n",ord);
  printf("state seq   : %d\n",sequence);
  printf("state onset : %d\n",onset);
  printf("state suit :");
  for(i=0;i<4;i++)printf("%d ",suit[i]);
  printf("\n"); printf("state player qty :");
  for(i=0;i<5;i++)printf("%d ",player_qty[i]);
  printf("\n"); printf("state player rank :");
  for(i=0;i<5;i++)printf("%d ",player_rank[i]);
  printf("\n"); printf("state player_num on seat :");
  for(i=0;i<5;i++)printf("%d ",seat[i]);
  printf("\n");
}

void DagentUEC_kowloon::initgame(int input[8][15]){
  pos.initgame();
  state.initgame();
  pos.setMyhand(input);
 
}


void DagentUEC_kowloon::receive_ply(int input[8][15]){
  //pos.setPly(input);
  state.getField(input);
  state.state2pos(pos);
}

void DagentUEC_kowloon::send_change(int input[8][15], int output[8][15]){
  // 一応ゼロクリアしとく
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      output[i][j] = 0;
    }
  }
#ifdef CONNECTION_DEBUG
  cout<<"send change input"<<endl;
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      cout<<input[i][j]<<" ";
    }
    cout<<endl;
  }
#endif
  change_result cr = change_kowloon::change(pos, psf, input[5][1]);
  for(int suit=0;suit<4;++suit){
    for(int rank=1;rank<14;++rank){
      if(cr.discard & (CARDS_HORIZON << (rank*4+suit))){
	output[/*3-*/suit][rank] = 1;
      }else{
	output[/*3-*/suit][rank] = 0;
      }
    }
  }
  
#ifdef CONNECTION_DEBUG
  cout<<"send change output"<<endl;  
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      cout<<output[i][j]<<" ";
    }
    cout<<endl;
  }
#endif
}

void DagentUEC_kowloon::search(int output[8][15]){
  // 一応ゼロクリアしとく
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      output[i][j] = 0;
    }
  }

#ifdef CONNECTION_DEBUG
  cout<<"position "<<endl;
  cout<<pos<<endl;
  cout<<"board"<<endl;
  cout<<pos.board<<endl;
#endif
  eval::setids(pos, psf);
  movepick::genmove(pos,moves);
  value_ps bestps(-1145141919);
  mover bestmove;
  
  if(moves.size() == 1){
    cout<<"onlypass"<<endl;
  }else if(state.passnum == 0 && pos.board.type != PASS){
    // ボードを取ったら即座にパスする
    cout<<"already getboard"<<endl;
  }else{
    for(auto mv : moves){
      // legalmoveがpass以外にあるのにpassすることを許可しない
      // 不適切なhackだが、評価関数だけで解決できそうもないぞ...
      /*
      if( mv.type == PASS){
	continue;
      }
      */
      
      value_ps ps = eval::staticeval_ps(pos, mv, psf);

      if(ps.value > bestps.value){
	bestps = ps;
	bestmove = mv;
      }
    }
  }
  mover2table(bestmove,output,pos.getMyhand());
  state.passnum = -1;
#ifdef CONNECTION_DEBUG
  cout<<"bestmove"<<endl;
  cout<<bestmove<<endl;
  cout<<"search output"<<endl;  
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      cout<<output[i][j]<<" ";
    }
    cout<<endl;
  }
#endif

}

void DagentUEC_kowloon::receive_board(int input[8][15]){
  state.getState(input);
  state.state2pos(pos);
  
  pos.setBoard(input);
}

void DagentUEC_kowloon::mover2table(mover &mv, int output[8][15], const uint64_t myhand){
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      output[i][j]=0;
    }
  }
  for(int suit = 0; suit<4;++suit){
    for(int i=1;i<14;++i){
      output[/*3-*/suit][i] = 0;
      if(mv.cards & (CARDS_HORIZON << (i*4+suit))){
	if(myhand & (CARDS_HORIZON << (i*4+suit))){
	  output[/*3-*/suit][i] = 1;
	}else{
	  if(!mv.usejk){
	    cout<<"warning invalid move!!!!!"<<endl;
	    cout<<mv<<endl;
	    visbit(cout, myhand, true);
	  }else{
	    output[/*3-*/suit][i] = 2;
	  }
	}
      }
    }
  }
  if(mv.usejk && mv.type == PAIR1){
    output[3][14] = 2; // joker単体
  }
}

void DagentUEC_kowloon::option(stringstream &ss){
  string token;
  ss >> token;
  if(token=="initgame"){
    int table[8][15];
    for(int i=0;i<8;++i){
      for(int j=0;j<15;++j){
	ss >> table[i][j];
      }
    }
    initgame(table);
  }else if(token=="receiveply"){
    int table[8][15];
    for(int i=0;i<8;++i){
      for(int j=0;j<15;++j){
	ss >> table[i][j];
      }
    }
    receive_ply(table);
  }else if(token=="receiveboard"){
    int table[8][15];
    for(int i=0;i<8;++i){
      for(int j=0;j<15;++j){
	ss >> table[i][j];
      }
    }
    receive_board(table);
  }else if(token=="init"){
    cout<<"initialize"<<endl;
    cout<<"initialize pairmasks "<<endl;
    movepick::initpairmasks();
    cout<<"initialize psf function "<<endl;
    eval::loadeval("evaltest", psf);
  }else if(token=="genmove"){
    cout<<"test for generation of legal moves"<<endl;
    cout<<pos<<endl;
    movepick::genmove(pos,moves);
    for(auto mv : moves){
      cout<<mv<<endl;
    }
  }else if(token=="setboard"){
    // boardのカードを決定する(legal moveやsearchのテスト用)
    // usage setboard kakumei(0/1) type(役のタイプ。0-9の数字 = pass, pair1-5, kaidan3-6) rank(1-13、階段の場合は最初の数字) loc(縛り。0-15の数字)
    int sand;
    ss >> pos.kakumei;
    ss >> sand;
    pos.board.type = handtype(sand);
    ss >> pos.board.rank;
    ss >> pos.board.suitlock;
  }else if(token=="translation"){
    transpos ttp;
    vector<transpos> tps;
    translation::gentrans(pos.getMyhand(), ttp, tps);
    // 手の解釈を追加
    for(auto tp : tps){
      cout<<"---translation---"<<endl;
      for(int i=0;i<4;++i){
	visbit(cout, tp.kaidan[i], false);
	cout<<endl;
      }
      visbit(cout, tp.reshand, false);
    }
  }else if(token=="search"){
    // 指定したposに於いてbestmoveを決定する
    movepick::genmove(pos,moves);
    eval::setids(pos, psf);
    for(auto mv : moves){
      cout<<mv<<endl;
      eval::staticeval_ps(pos, mv, psf);
    }
  }else if(token=="learneval"){
    string fname;
    ss >> fname;
    eval::loadeval(fname, learner::lpack.psf);
  }else if(token=="eval"){
    string fname;
    ss >> fname;
    eval::loadeval(fname, psf);
  }else if(token=="learn"){
    string fname;
    ss >> fname;
    cout<<"learn from "<<fname<<endl;
    learner::readTeacher(fname, pos);
    cout<<"learning done"<<endl;
  }else if(token=="visbit"){
    uint64_t bit;
    ss >> bit;
    visbit(cout, bit, true);
  }else if(token=="change"){
    int num_of_change;
    ss >> num_of_change;
    int output[8][15];
    change_kowloon::change(pos, psf, num_of_change);
  }
}
