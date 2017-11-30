#ifndef _INC_EVAL_H_
#define _INC_EVAL_H_

#include "mate.h"
#include "common/common.h"
#include "position.h"
#include "translation.h"

using namespace std;

// 評価関数の形の良し悪しがいまいち定まらない。
// とりあえずdefineでonoffしてみることに

// bprは意味をなしてるっぽいがpprとpsrは怪しい
#define USE_PPR
#define USE_BPR
#define USE_PSR

/*
  評価値の形式（案）
  大貧民における手の評価は手の解釈に近い。
  手札から合法な階段を取り除いた状態の集合translationと、ペア、階段毎の点数
  （状況によってペアの価値は変わるので、見えてない札との相関を取る）
  を定義し、最も評価値の高いtranslationを手札の点数とする。

*/  

const int p_max = 32 * 13; // 各rankにつき0/1で表現+jokerの有無
const int pe_max = 32 * 13; // 各rankにつき0/1で表現+jokerの有無

const int s_max = 4 * 13 * 4; // 各rankにつき1-4枚。3,4,5,6枚階段に対応
const int se_max = 5 * 13 * 4; // 各rankにつき0-4枚。3,4,5,6枚階段に対応

const int bp_max = 16 * 16 * 13; // 各rankにつき0/1で表現＋suitlock。出した時点でjokerは何らかの札になってるはずなので、16でok（joker単体や5枚ペアはあるけどな）
const int bp_pass = 0; // bpがpassの場合
const int bp_flushable = 1; // bpが場を取れることが確定の場合
const int bs_max = 4 * 13; // 3,4,5,6段階段、suit毎

// 手の評価値。残った手札と盤面の手札の強さで決定する
struct psfunc{

  psfunc(){

  }
  
  int ppjr[2][p_max][pe_max]; // 自分のペア、相手のペアから与えられる点数。一番最初の[2]は革命
  
#ifdef USE_PPR
  int ppr[2][p_max][p_max]; // 自分のペアの2駒関係
#endif
#ifdef USE_BPR
  int bpr[2][bp_max][p_max]; // 出した札と自分の手札の相関
#endif

#ifdef USE_PSR
  int psr[2][p_max][s_max]; // 自分のペアと階段の2駒関係
#endif
  
  int ssjr[2][s_max][se_max]; // 階段-階段
  
  int bpjr[2][bp_max][pe_max]; // 出した札の評価値。
  int bsjr[2][bs_max][se_max]; // 階段-階段の評価値（縛りは無視）

  vector<int> enemypairids; // enemypqrからpsfunc用のidを取得（仮
  vector<int> enemykaidanids; // enemykaidanからpsfunc用のidを取得（仮

};

// psfuncでの評価値
struct value_ps{
  value_ps(){
  }
  value_ps(int iv){
    value = iv;
  }
  value_ps(int iv, transpos it){
    value = iv;
    ttp = it;
  }
  long int value; // 評価値
  transpos ttp; // 最も良い評価値でのtranslation
};

// todo 指し手によって盤面が取れる確率をどうやって計算しよう
// mateとは別に相手が戦略的事情でパスすることはありえるわけで

namespace eval{

  // 評価関数ファイルの読み込み
  void loadeval(const string fname, psfunc &psf);

  // 評価関数ファイルの書き込み
  void saveeval(const string fname, psfunc &psf);
  
  // 見えてない手札から相手のidを取得する
  void setids(Position &pos, psfunc &psf);
    
  // 手持ちのカード、相手の残り札、自分が出した手から評価値を計算する
  // 将来的にohtoさんの棋譜から学ばせたいので其れを意識して設計すること
  // 軽量級ではstaticevalが最も高くなる手を指すようにすればよい（はず
  value_ps staticeval_ps(Position &pos, mover &mv, const psfunc &psf);

}

#endif
