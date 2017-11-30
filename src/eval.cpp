#include "eval.h"
#include "common/common.h"
#include "position.h"

using namespace std;

//#define EVAL_DEBUG

void eval::loadeval(const string fname, psfunc &psf){
  // yaneuraouを参考にbin形式にしたほうが良い気もするが...
  const string fnamepp = fname+"_ppjr.bin";
#ifdef USE_PPR
  const string fnameppr = fname+"_ppr.bin";
#endif
  const string fnamess = fname+"_ssjr.bin";
  const string fnamebp = fname+"_bpjr.bin";
  const string fnamebs = fname+"_bsjr.bin";
#ifdef USE_BPR
  const string fnamebpr = fname+"_bpr.bin";
#endif
#ifdef USE_PSR
  const string fnamepsr = fname+"_psr.bin";
#endif
  
  cout<<"load eval from "<<fnamepp<<","<<fnamess<<","<<fnamebp<<","<<fnamebs<<endl;
  ifstream ifs(fnamepp.c_str(), std::ios::binary);
  if (ifs) {
    ifs.read(reinterpret_cast<char*>(psf.ppjr), sizeof(int)*2*p_max*pe_max);
  }else{
    cout<<"err could not load "<<fnamepp;
  }
  
#ifdef USE_PPR
  ifstream ifs5(fnameppr.c_str(), std::ios::binary);
  if (ifs5) {
    ifs5.read(reinterpret_cast<char*>(psf.ppr), sizeof(int)*2*p_max*p_max);
  }else{
    cout<<"err could not load "<<fnameppr;
  }
#endif
  
  ifstream ifs2(fnamess.c_str(), std::ios::binary);
  if (ifs2) {
    ifs2.read(reinterpret_cast<char*>(psf.ssjr), sizeof(int)*2*s_max*se_max);
  }else{
    cout<<"err could not load "<<fnamess;
  }
  ifstream ifs3(fnamebp.c_str(), std::ios::binary);
  if (ifs3) {
    ifs3.read(reinterpret_cast<char*>(psf.bpjr), sizeof(int)*2*bp_max*pe_max);
  }else{
    cout<<"err could not load "<<fnamebp;
  }
  ifstream ifs4(fnamebs.c_str(), std::ios::binary);
  if (ifs4){
    ifs4.read(reinterpret_cast<char*>(psf.bsjr), sizeof(int)*2*bs_max*se_max);
  }else{
    cout<<"err could not load "<<fnamebs;
  }

#ifdef USE_BPR
  ifstream ifs6(fnamebpr.c_str(), std::ios::binary);
  if (ifs6) {
    ifs6.read(reinterpret_cast<char*>(psf.bpr), sizeof(int)*2*bp_max*p_max);
  }else{
    cout<<"err could not load "<<fnamebpr;
  }
#endif

#ifdef USE_PSR
  ifstream ifs7(fnamepsr.c_str(), std::ios::binary);
  if (ifs7) {
    ifs7.read(reinterpret_cast<char*>(psf.psr), sizeof(int)*2*p_max*s_max);
  }else{
    cout<<"err could not load "<<fnamepsr;
  }
#endif

}

void eval::saveeval(string fname, psfunc &psf){

    // yaneuraouを参考にbin形式にしたほうが良い気もするが...
  const string fnamepp = fname+"_ppjr.bin";
#ifdef USE_PPR
  const string fnameppr = fname+"_ppr.bin";
#endif
  const string fnamess = fname+"_ssjr.bin";
  const string fnamebp = fname+"_bpjr.bin";
  const string fnamebs = fname+"_bsjr.bin";
#ifdef USE_BPR
  const string fnamebpr = fname+"_bpr.bin";
#endif
#ifdef USE_PSR
  const string fnamepsr = fname+"_psr.bin";
#endif


  ofstream ofs(fnamepp.c_str(), std::ios::binary);
  if (ofs) ofs.write(reinterpret_cast<char*>(psf.ppjr), sizeof(int)*2*p_max*pe_max);
  
  ofstream ofs2(fnamess.c_str(), std::ios::binary);
  if (ofs2) ofs2.write(reinterpret_cast<char*>(psf.ssjr), sizeof(int)*2*s_max*se_max);
  ofstream ofs3(fnamebp.c_str(), std::ios::binary);
  if (ofs3) ofs3.write(reinterpret_cast<char*>(psf.bpjr), sizeof(int)*2*bp_max*pe_max);
  ofstream ofs4(fnamebs.c_str(), std::ios::binary);
  if (ofs4) ofs4.write(reinterpret_cast<char*>(psf.bsjr), sizeof(int)*2*bs_max*se_max);
  
#ifdef USE_PPR
  ofstream ofs5(fnameppr.c_str(), std::ios::binary);
  if (ofs5) ofs5.write(reinterpret_cast<char*>(psf.ppr), sizeof(int)*2*p_max*p_max);
#endif

#ifdef USE_BPR
  ofstream ofs6(fnamebpr.c_str(), std::ios::binary);
  if (ofs6) ofs6.write(reinterpret_cast<char*>(psf.bpr), sizeof(int)*2*bp_max*p_max);
#endif
#ifdef USE_PSR
  ofstream ofs7(fnamepsr.c_str(), std::ios::binary);
  if (ofs7) ofs7.write(reinterpret_cast<char*>(psf.psr), sizeof(int)*2*p_max*s_max);
#endif

  
}

void eval::setids(Position &pos, psfunc &psf){
  pos.genpqr_and_doms();
  
  psf.enemypairids.clear();
  psf.enemykaidanids.clear();
  
  // enemyhand, enemykaidanからidを取得
  uint64_t ehand = pos.getEnemyall();
  int jkb = (ehand & CARD_JOKER ? 16 : 0);
  for(int rnk=1;rnk<14;++rnk){
    uint64_t handbit = (ehand >> (rnk*4)) & 15;
    psf.enemypairids.push_back(32*(rnk-1)+handbit + jkb);
  }

  for(int kaidan = 0; kaidan<4; ++kaidan){
    for(int rnk=1;rnk<14;++rnk){
      int knum = 0;
      for(int num = 0; num<4;++num){
	if(pos.enemykaidan[0][kaidan] & (CARDS_HORIZON << (rnk*4+num)) || pos.enemykaidan[1][kaidan] & (CARDS_HORIZON << (rnk*4+num))){
	  knum++;
	}
      }
      psf.enemykaidanids.push_back(kaidan*13*5+(rnk-1)*5+knum);
    }
  }
#ifdef EVAL_DEBUG
  cout<<"enemypairids : ";
  for(auto id : psf.enemypairids){
    cout<<id<<",";
  }
  cout<<endl;
  cout<<"enemykaidanids : ";
  for(auto id : psf.enemykaidanids){
    cout<<id<<",";
  }
  cout<<endl;
#endif
}


value_ps eval::staticeval_ps(Position &pos, mover &mv, const psfunc &psf){

  // step1　指した手の処理
  // 合法手を指した後の手札
  // todo 注意：jokerなしで出せる役を意図的にjokerを使って出す（上がり制限がないuecルールではほぼ無意味）手が非合法手扱いになっている
  uint64_t afterhand = pos.getMyhand() & (~(mv.cards |  (mv.usejk ? CARD_JOKER : 0)));
  //uint64_t afterhand = pos.getMyhand() & (~(mv.cards));
  int kakumei = pos.kakumei ? 1 : 0; // 0で通常、1で革命  
  // 指した手が革命の条件を満たす場合、革命が発生する
  if(mv.type == PAIR4 || mv.type == PAIR5 || mv.type == STAIR5 || mv.type == STAIR6){
    kakumei = (kakumei == 0 ? 1 : 0 );
  }
  // 出したカードと相手の手札から得られる評価値
  // 階段については縛りを加味しないので現状スキップ
  // 言い訳：入れたほうが良いとは思うけど、学習できるか怪しいの...
  int suitlock = 0;
  long int bscore = 0;
  int mpid = -1145;
  if(mv.type < STAIR3){
    const uint64_t mvc = (mv.cards >> (mv.rank * 4)) & 15;
    suitlock = ( (pos.board.cards >> (pos.board.rank * 4)) & 15 ) == mvc ? mvc : 0;
#ifdef EVAL_DEBUG
    cout<<"hand suitlock : "<<suitlock<<endl;
#endif
    mpid = suitlock*16*13+mvc*13+mv.rank-1;
    if(mv.type==PASS){
      mpid = bp_pass;
    }
    /*
    if(matechecker::isGetboard(mv, pos)){
      mpid = bp_flushable;
    }
    */
    for(auto id : psf.enemypairids){
      bscore += psf.bpjr[kakumei][mpid][id];
    }      
  }else{
    const int kaidantype = mv.type - STAIR3;
    for(auto id : psf.enemykaidanids){
      bscore += psf.bsjr[kakumei][13*kaidantype+(mv.rank-1)][id];
    }
  }
  

  // step 2 階段の取り扱い毎に手を分類する
#ifdef EVAL_DEBUG 
  visbit(cout, afterhand, true);
#endif
  transpos ttp;
  vector<transpos> tps;
  translation::gentrans(afterhand, ttp, tps);
  
  // 残った手札に対して点数をつける
  // simd命令を使うと多分早いが、バグチェックがしんどいので暫定ベタ
  int maxscore = -114514;
  
  for(auto tp : tps){
#ifdef EVAL_DEBUG // debug
    // translationの表示
    cout<<"---translation---"<<endl;
    for(int i=0;i<4;++i){
      visbit(cout, tp.kaidan[i], false);
      cout<<endl;
    }
    visbit(cout, tp.reshand, false);
    
    cout<<"pairid : ";
#endif
    if(matechecker::isMate(mv, tp, pos)){
      // 其の手がmateになるならそれを指すようにする
      return value_ps(value_mate, ttp);
    }
    int score = bscore;
    int jkb = (tp.reshand & CARD_JOKER ? 16 : 0);
    
    // step 2.5 切った手札の2駒関係
#ifdef USE_BPR
    if(mv.type < STAIR3){
      for(int rnk2=1;rnk2<14;++rnk2){
	uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
	score += psf.bpr[kakumei][mpid][32*(rnk2-1)+handbit2+jkb];
      }
    }
#endif

    // step 3 残った手札の2駒関係
    for(int rnk=1;rnk<14;++rnk){
      uint64_t handbit = (tp.reshand >> (rnk*4)) & 15;
#ifdef EVAL_DEBUG // debug
      cout<<"("<<rnk<<","<<handbit<<"),";
#endif
      // 手札内のペア、ペア関数
#ifdef USE_PPR
      for(int rnk2=1;rnk2<14;++rnk2){
	uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
	score += psf.ppr[kakumei][32*(rnk-1)+handbit+jkb][32*(rnk2-1)+handbit2+jkb];
      }
#endif

      for(auto id : psf.enemypairids){
	score += psf.ppjr[kakumei][32*(rnk-1)+handbit+jkb][id];
      }
    }
#ifdef EVAL_DEBUG
    cout<<endl;
    cout<<"kaidanid : ";
#endif

    // step 4 残った手札の二駒関数（階段）
    for(int kaidan = 0; kaidan<4; ++kaidan){
      for(int rnk=1;rnk<14;++rnk){
	int knum = 0;
	for(int num = 0; num<4;++num){
	  if(tp.kaidan[kaidan] & (CARDS_HORIZON << (rnk*4+num))){
	    knum++;
	  }
	}
	if(knum > 0){
#ifdef EVAL_DEBUG
	  cout<<"("<<kaidan<<","<<rnk<<","<<knum<<") ,";
#endif
	  for(auto id : psf.enemykaidanids){
	    score += psf.ssjr[kakumei][13*4*kaidan+(rnk-1)*4+knum-1][id];
	  }

	  // 手札内のペアと階段の関数
#ifdef USE_PSR
	  for(int rnk2=1;rnk2<14;++rnk2){
	    uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
	    score += psf.psr[kakumei][32*(rnk2-1)+handbit2+jkb][13*4*kaidan+(rnk-1)*4+knum-1]; 
	  }
#endif
	}
      }
    }
    
#ifdef EVAL_DEBUG
    cout<<endl;
    cout<<"score : "<<score<<endl;
#endif

    // scoreが最も良いtranslationを使う
    if(score> maxscore){
      maxscore = score;
      ttp = tp;
    }
  }
  
  
  // 最終的な評価値を決定
#ifdef EVAL_DEBUG
  // 盤面取得由来の点数と残った手札の点数に分割
  cout<<"stat move"<<endl;
  cout<<mv<<endl;
  cout<<"bscore "<<bscore<<"," << "hscore "<<maxscore-bscore<<endl;
#endif
  return value_ps(maxscore, ttp);
}
