#ifndef _INC_AGENT_KOWLOON_UEC_H_
#define _INC_AGENT_KOWLOON_UEC_H_
#include "common/common.h"
#include "agent_uec.h"
#include "position.h"
#include "movepick.h"
#include "eval.h"
#include "learner.h"
#include "changer_kowloon.h"

// kowloonの思考部

struct state_type
{
  
  uint64_t eply; // 敵が今まで出した手
  
  int ord; // rank jokerは平場で14扱い 階段は出始めの札を入れる
  int sequence; // 階段なら1
  int qty; // ペアの場合のペア枚数、階段の場合は階段の枚数3-n
  int rev; // 革命
  int b11; // 11バック
  int lock; // 縛り
  int onset; // 札がないまっさらな状態か否か
  int suit[5]; // どのスートが出ているか？

  int player_qty[5]; // プレイヤーの手札
  int player_rank[5]; // プレイヤーの階級
  int seat[5]; // 席番号
  int joker; // 自分の手札のジョーカー
  
  // 初期化
  void initgame();
  void flush();
  void getState(int cards[8][15]);
  void getField(int cards[8][15]);
  void showState();
  // stateをboardに変換
  void state2pos(Position &pos);
  int passnum;
};

class DagentUEC_kowloon : public DagentUEC{
  state_type state; // res.kou2（uecデフォ？）から引っ張ってきた
  Position pos;
  vector<mover> moves;
  psfunc psf;
  
 public:
  // 通信3（自分が大富豪or富豪の時）カード交換を行う

  void send_change(int input[8][15],int output[8][15]);
  // 通信4 手番に何を提出するカードの決定
  void search(int output[8][15]);
  // 通信5 盤面情報の取得
  void receive_board(int input[8][15]);
  // 通信6 プレイヤーの指し手の取得
  void receive_ply(int net_table[8][15]);

  // ゲームの初期化
  void initgame(int net_table[8][15]);
  
  // ちょっと行儀が悪いが、string文字列を使って設定を外部から変えられるようにしておく。usiプロトコルリスペクト
  void option(stringstream &ss);

  // moverをuec形式の[8][15]の配列に変換する
  void mover2table(mover &mv, int output[8][15], const uint64_t myhand);
  
};

extern DagentUEC_kowloon kowloon;

#endif
