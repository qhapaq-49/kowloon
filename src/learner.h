#ifndef _INC_LEARNER_H_
#define _INC_LEARNER_H_

#include "eval.h"

using namespace std;
enum gradmethod{
  BONA_ADAGRAD, // bonaメソ、そしてadagrad
};

struct learnpack{
  learnpack(){
    hitrate = 0;
    kifs = 0;
    err = 0.0; // 誤差
    eta = 100.0;
    batchsize = 10000000;
    epoch = 0;
  }
  // 額数させる関数
  psfunc psf;
  // 勾配 adagradなどで用いる
  double ppjr_grad[2][p_max][pe_max];
#ifdef USE_PPR
  double ppr_grad[2][p_max][p_max];
#endif
#ifdef USE_BPR
  double bpr_grad[2][bp_max][p_max];
#endif
#ifdef USE_PSR
  double psr_grad[2][p_max][s_max];
#endif
  double ssjr_grad[2][s_max][se_max];
  double bpjr_grad[2][bp_max][pe_max];
  double bsjr_grad[2][bs_max][se_max]; 

  
  // 勾配の累計値
  double ppjr_gradsum[2][p_max][pe_max];
#ifdef USE_PPR
  double ppr_gradsum[2][p_max][p_max];
#endif
#ifdef USE_BPR
  double bpr_gradsum[2][bp_max][p_max];
#endif
#ifdef USE_PSR
  double psr_gradsum[2][p_max][s_max];
#endif

  double ssjr_gradsum[2][s_max][se_max];
  double bpjr_gradsum[2][bp_max][pe_max];
  double bsjr_gradsum[2][bs_max][se_max]; 

  
  int epoch; // 世代数
  uint64_t hitrate; // 正解の手を選べた数
  uint64_t kifs;
  uint64_t batchsize;
  double err; // 誤差
  double eta;
  string inpfile; // 初期値が格納されたファイル
  string outfile; // 学習結果を保存する際の名前
};

namespace learner{
  // posでmoveすることを教える(bonaメソ風味)
  bool add_teacher(Position &pos, mover &mv);
  // 評価関数の更新(isSaveで結果を保存する)
  void update(bool isSave);

  void add_grad(Position &pos, mover &mv, psfunc &psf, const transpos &tp, int grad);

  void readTeacher(string fname, Position &pos);
  void viewstat(bool flush);
  void mirror();
  extern learnpack lpack;
}

#endif
