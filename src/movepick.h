#ifndef _INC_STRAGETY_H_
#define _INC_STRAGETY_H_

#include "common/common.h"
#include "position.h"
using namespace std;


/*
  プレイヤーの持ち札、盤面毎の合法なペアの出し方を格納した構造体
  numはpairの数。最大4C2=6。maskは各スートを使うか否かを0/1で表現。
  4ビットの情報をnum個つなげた構造とする。
  例、スペード、ハート、ダイヤを持つ状態で2枚ペアの合法手
  num = 3
  mask = 1100/1010/0110/0000....
  例、スペード、ハート、ダイヤ、クラブを持つ状態でスペードの縛りが入った3枚ペア
  num = 3
  mask = 1110/1011/1101/0000.....
*/
struct pairmask{
  pairmask(){
    num = 0;
    mask = 0;
  }
  int num;
  uint64_t mask;
};

namespace movepick{
  // 出す枚数[1-4] x 手を出す側のbit（スート毎に持つ持たぬを0/1で表現＋jokerでsize=32）x 縛りに関する情報(size = 16)
  // データサイズが大したこと無いので、ゲーム開始時に初期化し、ゲーム中は呼び出すだけ
  extern vector< vector< vector< pairmask > > > pairmasks;
  void initpairmasks();
  void genmove(Position &pos, vector<mover> &mvs);
  void genmovebody(Position &pos, vector<mover> &mvs, handtype type);
  void decodepairmask(const pairmask &pm, vector<mover> &mvs, int rank, bool usejk, handtype type);
}

std::ostream &operator<<(std::ostream &os, const pairmask &pm);
std::ostream &operator<<(std::ostream &os, const mover &mv);

#endif
