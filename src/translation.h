#ifndef _INC_TRANSLATION_H_
#define _INC_TRANSLATION_H_

#include "common/common.h"
#include "position.h"

using namespace std;

// 手の解釈。階段として解釈する手とpqrからなる
struct transpos{
  transpos(){
    kaidan[0] = kaidan[1] = kaidan[2] = kaidan[3] = 0;
  }
  uint64_t kaidan[4]; // 3-6段階段として採用する手、以下同じ
  uint64_t reshand; // 残った手札
};

namespace translation{
  void gentrans(uint64_t hand, transpos &tp, vector<transpos> &tps);
}

#endif
