#ifndef _INC_POSITION_H_
#define _INC_POSITION_H_

#include "config.h"
#include "cardconstant.h"
#include "common/common.h"

// 手の種類。単品出しはpairと見なす
enum handtype{
  PASS,
  PAIR1,
  PAIR2,
  PAIR3,
  PAIR4,
  PAIR5,
  STAIR3,
  STAIR4,
  STAIR5,
  STAIR6, // 6以上の階段はそもそも計算していないのでpassするしかない。これ大丈夫か？
  ILLEAGAL, // kowloonには認識できなかった手。boardのhandtypeがこれに成ったらpassする他無い
};

namespace handtypefunc{
  // 場に出されたカードから手のtypeを決定する
  handtype getHandtype(const uint64_t hand);
}

// 合法手
struct mover{
  mover(){
    cards = 0;
    rank = 0;
    usejk = false;
    type = PASS;
  }
  mover(uint64_t incards, int inrank, bool inusejk, handtype intype){
    cards = incards;
    rank = inrank;
    usejk = inusejk;
    type = intype;
  }
  uint64_t cards; // myhandと同じ形式で表されたカード使うカード
  // 以下の情報はcardsと同値だが、盤面判断をする際に簡略化したほうが良いと思うので実装しとく
  int rank;
  bool usejk; // jokerを使うか否か
  handtype type;
};


// 現在場に出ているカード
struct onboard{
  void flush(); // 場を流す
  uint64_t cards; // myhandと同じ形式で表されたカード使うカード
  uint64_t suitlock; // 各スート毎に1ビット。0-15の値を持つ。
  // 以下の情報はcardsと同値だが、盤面判断をする際に簡略化したほうが良いと思うので実装しとく
  handtype type;
  int rank;
};


// position。ゲーム中に自分から見える盤面を保持する
struct Position{
  Position(){
    kakumei = false;
  }


  uint64_t myhandpqr; // 自分の手札を数字別に何枚持っているかに分類
  uint64_t myhandkaidan[2][4]; // 自分の手札の階段情報。[2]...jokerの利用あり/なし [4]...suit
  
  uint64_t enemypqr; // 敵の手札を数字別に何枚持っているかに分類
  uint64_t enemykaidan[2][4]; // 敵の手札の階段情報。[2]...jokerの利用あり/なし [4]...suit
  
  uint64_t enemykaidannd[2]; // 敵の手札の階段状況。[2]...jokerの利用あり/なし
  
  uint64_t enemynd[16]; // enemyallから作られる支配的な札（平場）
  uint64_t enemyndk[16]; // enemyallから作られる支配的な札（革命場）
  
  bool kakumei; // 革命か否か
  bool genpqr; // pqrが生成されているか

  onboard board; // 場に出ている札
  
  
  // 手札のセットを作る
  void setMyhand(const int table[8][15]);
  void setBoard(const int table[8][15]);
  void setEply(const uint64_t eply);
  void setPly(const int table[8][15]);
  void addCards(int h, int w);
  void initgame();
  uint64_t getMyhand() const;
  uint64_t getMyply() const;
  uint64_t getEnemyply(int id) const;
  uint64_t getEnemyall() const;

  void setPos(uint64_t inmyhand, uint64_t inmyply, uint64_t inenemyply[4], bool inkakumei, uint64_t cboard, uint64_t suitlock, int type, int rank);
  void genpqr_and_doms();
  
private:

  uint64_t myhand; // 自分の手札
  uint64_t myply; // 自分が切ってきた札
  uint64_t enemyall; // まだ見えてない札
  uint64_t enemyply[4]; // 敵が既に出した札
  bool enemypass[4]; // 敵がpassをしているか。今の所使わない
  uint64_t prevsuit; // 前のターンに出た札のスート。suitlockに必要
};

void visbit(std::ostream &os, const uint64_t bit, bool jk);
std::ostream &operator<<(std::ostream &os, const Position &pos);
std::ostream &operator<<(std::ostream &os, const onboard &board);

#endif
