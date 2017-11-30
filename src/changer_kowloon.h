#ifndef _INC_CHANGER_KOWLOON_H_
#define _INC_CHANGER_KOWLOON_H_

#include "eval.h"
#include "position.h"


struct change_result{
change_result(){
discard = 0;
ps = value_ps(-114514);

}
  uint64_t discard; //切るカード
  value_ps ps;
};

namespace change_kowloon{
  // 指定した手札、指定した枚数、指定した評価関数で切るべき札を決める
  change_result change(Position &pos, psfunc &psf, int num_of_change);
  change_result recursive_eval(const uint64_t defbit, uint64_t bit, psfunc &psf, int num_of_change);
}

#endif
