#include "position.h"
#include "cardconstant.h"
#include "bithand.h"

#define CONNECTION_DEBUG

using namespace std;

void Position::setPos(uint64_t inmyhand, uint64_t inmyply, uint64_t inenemyply[4], bool inkakumei, uint64_t cboard, uint64_t suitlock, int type, int rank){
  myhand = inmyhand;
  myply = inmyply;
  for(int i=0;i<4;++i){
    enemyply[i] = inenemyply[i];
  }
  kakumei = inkakumei;
  board.cards = cboard;
  board.suitlock = suitlock;
  // この辺の情報は重複してるのだが、kowloon内部でconvertするのダルいので
  board.type =(handtype)type;
  board.rank =rank;
  // 手札を更新したら必ずpqrなどを更新する
  genpqr_and_doms();
}



void Position::setMyhand(const int table[8][15]){
  myhand = CARDS_NULL;
  for (int h = 0; h < 4; ++h){
    for (int w = 1; w < 14; ++w){
      if (table[h][w]){
	addCards(h,w);
      }
    }
  }
  if (table[4][1] != 0){
    myhand = myhand | CARDS_HORIZON << JOKER_NUM;
  }
  // 手札を更新したら必ずpqrなどを更新する
  genpqr_and_doms();

}

void onboard::flush(){
  cout<<"flushed"<<endl;
  cards = 0;
  suitlock = 0;
  rank = 0;
  type = PASS;
}

void Position::initgame(){
  myply = CARDS_NULL;
  enemyply[0] = enemyply[1] = enemyply[2] = enemyply[3] = CARDS_NULL;
  board.flush();
  kakumei = false;
}

void Position::setEply(const uint64_t eply){
  enemyply[0] = enemyply[0] | eply; // 手を指したplayeridは評価関数に今の所使わない
}

// uecプロトコルに従った場札の更新
void Position::setPly(const int table[8][15]){
#ifdef CONNECTION_DEBUG
  cout<<"setply"<<endl;
  for(int i=0;i<8;++i){
    for(int j=0;j<15;++j){
      cout<<table[i][j]<<" ";
    }
    cout<<endl;
  }
#endif
  
  // todo 場のカードとは今までに出された全ての札を意味するのか？ uecプロトコル漠然とし過ぎでねーか？
  uint64_t enemyplyall = enemyply[0] + enemyply[1] + enemyply[2] + enemyply[3];
  uint64_t plyhand = 0; //切られた手の一覧
  uint64_t plysuit = 0;
  int plyrank = 0;
  bool isjk = false;
  int jkid = -1;
  for (int h = 0; h < 5; ++h){
    for (int w = 0; w < 15; ++w){
      if(h==4 && (w==0 || w== 14)){continue;}
      if(table[h][w] == 2){
	isjk = true;
      }
      
      if(w == 0 || w == 14 || h==4){continue;}
   
      // plyされてない手札を列挙
      if (table[h][w] != 0){
	if(table[h][w] == 2){
	  jkid = ( (w << 2) + (/*3-*/h));
	}
	
	plyhand += CARDS_HORIZON << ( (w << 2) + (/*3-*/h));
	plysuit += (1 << (/*3-*/h));
	if(plyrank == 0){
	  plyrank = w;
	}
      }
    }
  }
  if(board.type == PASS){
    board.type = handtypefunc::getHandtype(plyhand);
  }
  if(isjk){
    if(jkid != -1){
      plyhand -= CARDS_HORIZON << jkid;
    }
    plyhand += CARD_JOKER;
  }
  cout<<"plyhand"<<endl;
  visbit(cout,plyhand,true);
  if(plyhand != 0){
    board.cards = plyhand;
    board.suitlock = board.suitlock & (prevsuit & plysuit);
    board.rank = plyrank;
    prevsuit = plysuit;
    
    enemyply[0] = enemyply[0] | (enemyall & plyhand);
    enemyall = enemyall & (CARD_ALL - plyhand);
  }
}

void Position::setBoard(const int table[8][15]){
  setMyhand(table);
  // 場が流れたか
  if(table[5][4] == 1){
    board.flush();
  }
  // kakumei
  if(table[5][6] == 1){
    kakumei = true;
  }else{
    kakumei = false;
  }
}


void Position::addCards(int h, int w){
  uint64_t r = CARDS_HORIZON << ( (w << 2) + (/*3-*/h));
  myhand = myhand | r;
}

void Position::genpqr_and_doms(){
  enemyall = CARD_ALL & (~myhand) & (~myply);
  for(int i=0;i<4;++i){enemyall = enemyall & (~enemyply[i]);}
  myhandpqr =  bithand::convpqr(myhand);
  enemypqr =  bithand::convpqr(enemyall);
  bithand::convkaidans(myhand & ~CARD_JOKER, myhand & CARD_JOKER, myhandkaidan);
  bithand::convkaidans(enemyall & ~CARD_JOKER, enemyall & CARD_JOKER, enemykaidan);
  bithand::convkaidandm(enemykaidan, enemykaidannd);
  for(uint64_t i=0;i<16;++i){
    uint64_t temp = bithand::suitlockfilter(enemyall,i);
    uint64_t tmppqr = bithand::convpqr(temp);
    enemynd[i] = bithand::convdominance(tmppqr & ~CARD_JOKER, enemyall & CARD_JOKER );
    enemyndk[i] = bithand::reverse(bithand::convdominance(bithand::reverse(tmppqr & ~CARD_JOKER), enemyall & CARD_JOKER ));
  }
  
}

uint64_t Position::getMyhand() const{
  return myhand;
}

uint64_t Position::getMyply() const{
  return myply;
}

uint64_t Position::getEnemyply(int id) const{
  return enemyply[id];
}

uint64_t Position::getEnemyall() const{
  return enemyall;
}


std::ostream &operator<<(std::ostream &os, const Position &pos){
  os << "Position : " << endl;
  os << "myhand : "<<endl;
  visbit(os, pos.getMyhand(), true);
  /*
  os << "myhandpqr : "<<endl;
  visbit(os, pos.myhandpqr, false);
  
  for(int idxk=0;idxk<8;++idxk){
    os << "myhandkaidan["<<idxk<<"] : "<<endl;
    visbit(os, pos.myhandkaidan[idxk/4][idxk%4],false);
  }
  */
  os << "myply : "<<endl;
  visbit(os,pos.getMyply(),true);
  
  for(int i=0;i<4;++i){
    os <<"enemyply : "<<endl;
    visbit(os,pos.getEnemyply(i),true);
  }
  os << "enemyall : "<<endl;
  visbit(os,pos.getEnemyall(),true);

  os << "enemypqr : "<<endl;
  visbit(os,pos.enemypqr, false);

  os << "enemynd : "<<endl;
  visbit(os,pos.enemynd[0], false);

  os << "enemyndk : "<<endl;
  visbit(os,pos.enemyndk[0], false);


  /*
  os << "enemypqr : "<<endl;
  visbit(os,pos.enemypqr, false);

  os << "enemynd : "<<endl;
  visbit(os,pos.enemynd, false);
  
  for(int idxk=0;idxk<8;++idxk){
    os << "enemykaidan["<<idxk<<"] : "<<endl;
    visbit(os, pos.enemykaidan[idxk/4][idxk%4],false);
  }

  for(int idxk=0;idxk<2;++idxk){
    os << "enemykaidannd["<<idxk<<"] : "<<endl;
    visbit(os, pos.enemykaidannd[idxk],false);
  }
  */
  os << "kakumei "<<pos.kakumei<<endl;
  return os;
}

std::ostream &operator<<(std::ostream &os, const onboard &board){
  visbit(os,board.cards,true);
  os<<endl;
  os<<"suitlock "<<board.suitlock<<endl;
  os<<"handtype "<<int(board.type)<<endl;
  os<<"rank "<<board.rank<<endl;
  return os;
}

// uint型を0/1に変換する
void visbit(std::ostream &os, const uint64_t bit, bool jk){
  for(int suit = 0; suit<4;++suit){
    for(int i=1;i<14;++i){
      if(bit & (CARDS_HORIZON << (i*4+suit))){
	os<<"1 ";
      }else{
	os<<"0 ";
      }
    }
    os<<endl;
  }
  if(jk){
    if(bit & (CARDS_HORIZON << JOKER_NUM)){
      os<<"1"<<endl;
    }else{
      os<<"0"<<endl;
    }
  }
}

handtype handtypefunc::getHandtype(const uint64_t plyhand){
  if(plyhand == 0){return PASS;}
  
  uint64_t plykaidans[2][4] = {};

  bithand::convkaidans(plyhand, false, plykaidans);

  // 段数の高い階段から探索する
  for(int i=0;i<4;++i){
    if(plykaidans[0][/*3-*/i] != 0){
      return handtype(int(STAIR6)-i);
    }
  }
  // 階段ではなさそうなので、pqrを探索する
  int pairnum = 0;
  for(int i=0;i<4;++i){
    if (plyhand & (bit0001 << i)){
      pairnum += 1;
    }
  }
  if(pairnum > 0){
    return handtype((pairnum-1)+int(PAIR1));
  }
  cout<<"hand illeagal!!"<<endl;
  visbit(cout,plyhand,true);
  return ILLEAGAL;
}
