#include "position.h"
#include "changer_kowloon.h"
#include "eval.h"

//#define CHANGER_DEBUG


change_result change_kowloon::change(Position &pos, psfunc &psf,int num_of_change){
#ifdef CHANGER_DEBUG
  cout<<"change"<<endl;
#endif
  // 手札をnum of change枚削ったときの評価関数が最も高いものを採択
  change_result cr = recursive_eval(pos.getMyhand(), pos.getMyhand(), psf, num_of_change);
#ifdef CHANGER_DEBUG
  visbit(cout, cr.discard, true);
#endif
  return cr;
}

change_result change_kowloon::recursive_eval(const uint64_t defbit, uint64_t bit, psfunc &psf,int num_of_change){
  if(num_of_change == 0){
    Position tmppos;
    change_result cr;
    uint64_t eply[4] = {0,0,0,0};
    tmppos.setPos(bit, 0, eply, false, 0,0,0,0);
    eval::setids(tmppos, psf);
    mover mv = mover(0,0,false,PASS);
    value_ps ps = eval::staticeval_ps(tmppos, mv, psf);
    cr.discard = defbit - bit;
    cr.ps = ps;
    return cr;
  }

  change_result sumcr;
  for(int suit = 0; suit<4;++suit){
    for(int i=1;i<14;++i){
      if(bit & (CARDS_HORIZON << (i*4+suit))){
	bit -= (CARDS_HORIZON << (i*4+suit));
	num_of_change -= 1;
	change_result cr = recursive_eval(defbit, bit, psf, num_of_change);
	if(sumcr.ps.value < cr.ps.value){
	  sumcr = cr;
	}
	bit += (CARDS_HORIZON << (i*4+suit));
	num_of_change += 1;
      }
    }
  }
  return sumcr;
}
