#ifndef _INC_STRAGETY_H_
#define _INC_STRAGETY_H_

#include "common/common.h"
#include "position.h"
using namespace std;

struct strategy{
  vector<uint64_t> nds; // non dominantな札達
  vector<uint64_t> ds; // 今の場でdominantな札達
  vector<uint64_t> revs; // 革命を起こせる手
  vector<uint64_t> dsrs; // 革命が起きている場合dominantな手達。revsのsizeが0の時はこいつも実質ndsなことに注意
  bool isMate; // 確定勝ちがあるか
};

namespace strategymaker{
  /* 
     手札をdominantな手、そうでない手に分割する。
     階段、ペアなどを駆使してできるだけ浮き札を減らすようにする。
     最終的にはstrategyを何らかの評価関数に入れることでどの手を指すかを決める予定
     ペアや階段を崩すことをどうやって評価するかが課題
  */  
  strategy getStrategy(Position &pos);

}

#endif
