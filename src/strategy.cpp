#include "strategy.h"

strategy strategymaker::getStrategy(Position &pos){
  // 手を階段として使うか、ジョーカーをどう処理するかによって多様性が生じる
  // 高速に判別するにはどうしたら良いだろう...
  // nd[i][0/1] ... i番目までの手札を使い切りジョーカーを使った状態でのndの数として動的計画法を使いたい?
  strategy st;
  return st;
}
