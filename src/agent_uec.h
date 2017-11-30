#ifndef _INC_AGENT_UEC_H_
#define _INC_AGENT_UEC_H_
#include "common/common.h"

using namespace std;
// 大貧民エージェントのプロトタイプ for UEC
class DagentUEC{
 public:
  // 配列の参照渡しについては以下を参考。メモリ破壊を誘発しそうで嫌なのだが、connect.cを流用したいので我慢
  // http://murakan.cocolog-nifty.com/blog/2011/08/cc-b731.html
  
  // 通信3（自分が大富豪or富豪の時）カード交換を行う
  virtual void send_change(int input[8][15], int output[8][15])=0;
  // 通信4 手番に何を提出するカードの決定
  virtual void search(int output[8][15])=0;
  // 通信5 盤面情報の取得
  virtual void receive_board(int net_table[8][15])=0;
  // 通信6 プレイヤーの指し手の取得
  virtual void receive_ply(int net_table[8][15])=0;
  
  // ちょっと行儀が悪いが、string文字列を使って設定を外部から変えられるようにしておく。usiプロトコルリスペクト
  virtual void option(stringstream &ss)=0;

  // ゲームを初期化する
  virtual void initgame(int net_table[8][15])=0;
  
};

#endif
