#include "translation.h"
#include "bithand.h"

using namespace std;

void translation::gentrans(uint64_t hand, transpos &tp, vector<transpos> &tps){
  uint64_t handkaidan[2][4]={{0,0,0,0},{0,0,0,0}};
  const uint64_t kaidanbit[4] = {273,4369,69905,1118481};
  bithand::convkaidans(hand & ~CARD_JOKER, hand & CARD_JOKER, handkaidan);
  for(int i=0;i<2;++i){
    for(int j=0;j<4;++j){
      for(int suit = 0; suit<4;++suit){
	for(int rnk=1;rnk<14;++rnk){
	  if(handkaidan[i][j] & (CARDS_HORIZON << (rnk*4+suit))){
	    uint64_t nexthand = hand;
	    nexthand &= ~(kaidanbit[j] << (rnk*4+suit));
	    if(i==1){
	      nexthand -= CARD_JOKER;
	    }
	    tp.kaidan[j] += (CARDS_HORIZON << (rnk*4+suit));
	    gentrans(nexthand, tp, tps);
	    tp.kaidan[j] -= (CARDS_HORIZON << (rnk*4+suit));
	  }
	}
      }
    }
  }
  tp.reshand = hand;
  tps.push_back(tp);
}
