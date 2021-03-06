#include "mate.h"
#include "movepick.h"
#include "bithand.h"

//#define MATE_DEBUG

using namespace std;

bool matechecker::isGetboard(mover &mv, Position &pos){
  
  // あんまり強くならないみたいなのでペンディング
  const uint64_t mvc = (mv.cards >> (mv.rank * 4)) & 15;
  int suitlock = ( (pos.board.cards >> (pos.board.rank * 4)) & 15 ) == mvc ? mvc : 0;
  
  //const int suitlock = 0;
  
  if(!(mv.cards & INC_8)){
    if(pos.kakumei){
      if((mv.type > PASS && mv.type < STAIR3 && (bithand::convpqr(mv.cards) & pos.enemyndk[suitlock])) || mv.type == PASS){
	return false;
      }      
    }else{
      if((mv.type > PASS && mv.type < STAIR3 && (bithand::convpqr(mv.cards) & pos.enemynd[suitlock])) || mv.type == PASS){
	return false;
      }
    }
  }
  return true;
}

bool matechecker::isMate(mover &mv, transpos &tp, Position &pos){
  
  // mateの条件は本当はもっと複雑なのだが、簡単そうなのから作ろう...
  // case1. 出したら手札がなくなる。勝ちである
  uint64_t afterhand = pos.getMyhand() & (~(mv.cards |  (mv.usejk ? CARD_JOKER : 0)));
  if(afterhand == 0){
#ifdef MATE_DEBUG
    cout<<"plymate!"<<endl;
    cout<<mv<<endl;
#endif
    return true;
  }
  
  // 手札がdominant、または8を含む(8切り)が確実か? noならfalse
  // jokerがある系は面倒だから排除
  // todo このplyでボードが取れるか否かはsuitlockも絡むので本来は加味するべきだ
  if(!isGetboard(mv, pos)){
    return false;
  }

  // 残った手札のうち、dominantではない8ではないpqrを列挙
  int nondoms = getNodompair(bithand::convpqr(tp.reshand), (pos.kakumei ? pos.enemyndk[0] : pos.enemynd[0]));
  // 実際はkaidanはdominantではないのだが、ヒューリスティックにdominantとして良い気がする
   
  // dominantでない手の数が1ならmate
  if(nondoms < 2){
#ifdef MATE_DEBUG
    cout<<"dominant mate "<<nondoms<<endl;
    cout<<pos<<endl;
    cout<<pos.board<<endl;
    visbit(cout,tp.reshand,true);
    visbit(cout,bithand::convpqr(tp.reshand),false);
    visbit(cout,(pos.enemynd & bithand::convpqr(tp.reshand)),false);
    cout<<mv<<endl;
#endif
    
    return true;
  }
  return false;
}

int matechecker::getNodompair(uint64_t handpqr, uint64_t enemypqr){
  const uint64_t checker = handpqr & enemypqr;
  int out = 0;
  for(int i=1;i<14;++i){
    if(i==6){
      continue; //nodom checkでは8を加味する必要はない
    }
    if(checker & (CARDS_1SUIT << (i*4))){
      out+=1;
    }
  }
  return out;
}
