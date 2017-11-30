#include "learner.h"
#include "movepick.h"
learnpack learner::lpack;

//#define LEARN_DEBUG

//#define LIMIT_VALUE

bool learner::add_teacher(Position &pos, mover &mv){
  eval::setids(pos, lpack.psf);
  vector<mover> moves;
  movepick::genmove(pos,moves);
#ifdef LEARN_DEBUG
  cout<<"genmoves done movesize : "<<moves.size()<<endl;
#endif
  value_ps pvs = eval::staticeval_ps(pos, mv, lpack.psf);
  // mateだったら教師から外す
  
  if (pvs.value == value_mate){
    return true;
  }
  
#ifdef LEARN_DEBUG
  cout<<"staticeval done value : "<<pvs.value<<endl;
#endif
  bool hit = true;
  // passしか合法手がない場合を教師から外す
  if(moves.size()==1){
    return true;
  }
  bool selfcheck = false;
  const int bonawindow = 8192;
  for(auto mov : moves){
    if( (mov.cards & CARD_52) == ( mv.cards & CARD_52) && mov.type == mv.type){selfcheck = true; continue;}
    value_ps npvs = eval::staticeval_ps(pos, mov, lpack.psf);
    if (npvs.value == value_mate){
      /*
      cout<<"mate?"<<endl;
      cout<<pos<<endl;
      cout<<pos.board<<endl;
      cout<<mov<<endl;
      */
      continue;

    }
    int vdiff = pvs.value - npvs.value - bonawindow;
    // スコア差を比較し、差に応じて評価値を弄る
    if(vdiff <= 0){
      lpack.err += vdiff * vdiff;
      add_grad(pos, mov, lpack.psf, npvs.ttp, vdiff);
      add_grad(pos, mv, lpack.psf, pvs.ttp, -vdiff);
      if(pvs.value - npvs.value <= 0){
	hit = false;
      }
#ifdef LEARN_DEBUG
      cout<<"add value "<<vdiff<<endl;
      cout<<mov<<endl;
#endif
    }
  }
  if(!selfcheck){
    if(mv.type!=PAIR5){
      cout<<"pv is illeagal or pos is broken!!"<<endl;
      cout<<pos<<endl;
      cout<<pos.board<<endl;
      cout<<mv<<endl;
      cout<<"lmoves"<<endl;
      for(auto mov : moves){
	cout<<mov<<endl;
      }
      return false;
    }
  }
  if(hit){
    // 合法手がjoker単品pushしかない場合だけ、evalが全て0でもbestmoveが当たる。なぜだ？
    // evalが全て0だとbestmoveはpassになる。意図的passをする局面は当たるがな
    lpack.hitrate += 1;
    
  }else{
    /*
    // 当たらなかった手を列挙してみる
    cout<<pos<<endl;
    cout<<pos.board<<endl;
    cout<<mv<<endl;
    cout<<"moves"<<endl;
    for(auto mov : moves){
      value_ps npvs = eval::staticeval_ps(pos, mov, lpack.psf);
      cout<<mov<<endl;
      cout<<npvs.value<<endl;   
    }
    */
  
  }
  lpack.kifs+=1;
  if(lpack.kifs > lpack.batchsize){
    lpack.epoch += 1;
    // todo epochで条件つけするべし
    viewstat(true);
    update(true);
  }
  return true;
}

void learner::viewstat(bool flush){
  cout <<"epoch "<<lpack.epoch<<" "
       <<"hitrate " <<lpack.hitrate << "/" << lpack.kifs
       <<" err "<<lpack.err<<endl;
  if(flush){
    lpack.hitrate = 0;
    lpack.kifs = 0;
    lpack.err = 0;
  }
}

void learner::add_grad(Position &pos, mover &mv, psfunc &psf, const transpos &tp, int grad){
  
  int kakumei = (pos.kakumei ? 1 : 0);
  int jkb = (tp.reshand & CARD_JOKER ? 16 : 0);
  int mpid = -1145;
  // bprの更新
#ifdef USE_BPR
  if(mv.type < STAIR3){
    const uint64_t mvc = (mv.cards >> (mv.rank * 4)) & 15;
    int suitlock = ( (pos.board.cards >> (pos.board.rank * 4)) & 15 ) == mvc ? mvc : 0;
    mpid = suitlock*16*13+mvc*13+mv.rank-1;
    if(mv.type==PASS){
      mpid = bp_pass;
    }
    
    if(matechecker::isGetboard(mv, pos)){
      mpid = bp_flushable;
    }
    
    for(int rnk2=1;rnk2<14;++rnk2){
      uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
      lpack.bpr_grad[kakumei][mpid][32*(rnk2-1)+handbit2+jkb] += grad;
    }
  }
#endif

  
  // ppjrについて
  for(int rnk=1;rnk<14;++rnk){
    uint64_t handbit = (tp.reshand >> (rnk*4)) & 15;
    
#ifdef USE_PPR
    for(int rnk2=1;rnk2<14;++rnk2){
      uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
      lpack.ppr_grad[kakumei][32*(rnk-1)+handbit+jkb][32*(rnk2-1)+handbit2+jkb] += grad;
    }
#endif

    for(auto id : psf.enemypairids){
      lpack.ppjr_grad[kakumei][32*(rnk-1)+handbit+jkb][id] += grad;
    }
  }
  
  // ssjrについて
  for(int kaidan = 0; kaidan<4; ++kaidan){
    for(int rnk=1;rnk<14;++rnk){
      int knum = 0;
      for(int num = 0; num<4;++num){
	if(tp.kaidan[kaidan] & (CARDS_HORIZON << (rnk*4+num))){
	  knum++;
	}
      }
      if(knum > 0){
	for(auto id : psf.enemykaidanids){
	  lpack.ssjr_grad[kakumei][13*4*kaidan+(rnk-1)*4+knum-1][id] += grad;
	}
	      // 手札内のペアと階段の関数
#ifdef USE_PSR
	for(int rnk2=1;rnk2<14;++rnk2){
	  uint64_t handbit2 = (tp.reshand >> (rnk2*4)) & 15;
	  lpack.psr_grad[kakumei][32*(rnk2-1)+handbit2+jkb][13*4*kaidan+(rnk-1)*4+knum-1] += grad; 
	}
#endif	
      }
    }
  }

  // bpjr,bsjrについて
  if(mv.type < STAIR3){
    const uint64_t mvc = (mv.cards >> (mv.rank * 4)) & 15;
    int suitlock = (pos.board.cards >> (pos.board.rank * 4)) & mvc;

    for(auto id : psf.enemypairids){
      lpack.bpjr_grad[kakumei][mpid][id] += grad;
    }
    
  }else{
    const int kaidantype = mv.type - STAIR3;
    for(auto id : psf.enemykaidanids){
      lpack.bsjr_grad[kakumei][13*kaidantype+(mv.rank-1)][id] += grad;
    }
  }
}


void learner::mirror(){
  // 対称性を加味して評価関数をならす
  vector< vector<int> > equivs = {{0},{1,2,4,8},{3,5,9,6,10,12},{7,11,13,14},{15}};
  for(int i=0;i<2;++i){
    for(int rnk1=0;rnk1<13;++rnk1){
      for(int rnk2=0;rnk2<13;++rnk2){
	for(auto jkl : {0,16}){
	  for(auto jkl2 : {0,16}){
	    for(auto eq1 : equivs){
	      for(auto eq2 : equivs){
		int total = 0;
		int cnt = 0;
		for(auto e1 : eq1){
		  for(auto e2 : eq2){
		    if(e1==e2){continue;}
		    total += lpack.psf.ppjr[i][32*rnk1+e1+jkl][32*rnk2+e2+jkl2];
		    cnt+=1;
		  }
		}
		for(auto e1 : eq1){
		  for(auto e2 : eq2){
		    if(e1==e2){continue;}
		    lpack.psf.ppjr[i][32*rnk1+e1+jkl][32*rnk2+e2+jkl2] = total/cnt;
		    
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  for(int i=0;i<2;++i){
    for(int rnk1=0;rnk1<13;++rnk1){
      for(auto suitlock : {false, true}){
	for(int rnk2=0;rnk2<13;++rnk2){
	  for(auto jkl2 : {0,16}){
	    for(auto eq1 : equivs){
	      for(auto eq2 : equivs){
		int total = 0;
		int cnt = 0;
		for(auto e1 : eq1){
		  for(auto e2 : eq2){
		    if(e1==e2){continue;}
		    total += lpack.psf.bpjr[i][(suitlock ? e1 : 0)*16*13+e1*13+rnk1][32*rnk2+e2+jkl2];
		    cnt+=1;
		  }
		}
		
		for(auto e1 : eq1){
		  for(auto e2 : eq2){
		    if(e1==e2){continue;}
		    lpack.psf.bpjr[i][(suitlock ? e1 : 0)*16*13+e1*13+rnk1][32*rnk2+e2+jkl2] = total/cnt;
		  }
		}	
	      }
	    }
	  }
	}
      }
    }
  }
}

void learner::update(bool isSave){
  // adagradで更新
  // adagradは将棋では微妙だったしadadeltaにしようかな
  const double epsilon = 0.01;
  for(int i=0;i<2;++i){
    for(int j=0;j<p_max;++j){
      for(int k=0;k<pe_max;++k){
	lpack.ppjr_gradsum[i][j][k] += lpack.ppjr_grad[i][j][k] * lpack.ppjr_grad[i][j][k];
	lpack.ppjr_gradsum[i][j][k] = 0.9999 * lpack.ppjr_gradsum[i][j][k];
	lpack.psf.ppjr[i][j][k] += lpack.eta * lpack.ppjr_grad[i][j][k] / sqrt(lpack.ppjr_gradsum[i][j][k] + epsilon);
#ifdef LIMIT_VALUE
	if(lpack.psf.ppjr[i][j][k] > 0){
	  lpack.psf.ppjr[i][j][k] = 0;
	}
#endif
	lpack.ppjr_grad[i][j][k] = 0;
      }
    }
  }
  
#ifdef USE_PPR
  for(int i=0;i<2;++i){
    for(int j=0;j<p_max;++j){
      for(int k=0;k<p_max;++k){
	lpack.ppr_gradsum[i][j][k] += lpack.ppr_grad[i][j][k] * lpack.ppr_grad[i][j][k];
	lpack.ppr_gradsum[i][j][k] = lpack.ppr_gradsum[i][j][k] * 0.9999;
	lpack.psf.ppr[i][j][k] += lpack.eta * lpack.ppr_grad[i][j][k] / sqrt(lpack.ppr_gradsum[i][j][k] + epsilon);
	lpack.ppr_grad[i][j][k] = 0;
      }
    }
  }
#endif
  
#ifdef USE_BPR
  for(int i=0;i<2;++i){
    for(int j=0;j<bp_max;++j){
      for(int k=0;k<p_max;++k){
	lpack.bpr_gradsum[i][j][k] += lpack.bpr_grad[i][j][k] * lpack.bpr_grad[i][j][k];
	lpack.bpr_gradsum[i][j][k] = lpack.bpr_gradsum[i][j][k] * 0.9999;
	lpack.psf.bpr[i][j][k] += lpack.eta * lpack.bpr_grad[i][j][k] / sqrt(lpack.bpr_gradsum[i][j][k] + epsilon);
	lpack.bpr_grad[i][j][k] = 0;
      }
    }
  }

#endif

#ifdef USE_PSR
  for(int i=0;i<2;++i){
    for(int j=0;j<p_max;++j){
      for(int k=0;k<s_max;++k){
	lpack.psr_gradsum[i][j][k] += lpack.psr_grad[i][j][k] * lpack.psr_grad[i][j][k];
	lpack.psr_gradsum[i][j][k] = lpack.psr_gradsum[i][j][k] * 0.9999;
	lpack.psf.psr[i][j][k] += lpack.eta * lpack.psr_grad[i][j][k] / sqrt(lpack.psr_gradsum[i][j][k] + epsilon);
	lpack.psr_grad[i][j][k]=0;
      }
    }
  }

#endif

  
  for(int i=0;i<2;++i){
    for(int j=0;j<s_max;++j){
      for(int k=0;k<se_max;++k){
	lpack.ssjr_gradsum[i][j][k] += lpack.ssjr_grad[i][j][k] * lpack.ssjr_grad[i][j][k];
	lpack.ssjr_gradsum[i][j][k] = 0.9999 * lpack.ssjr_gradsum[i][j][k];
	lpack.psf.ssjr[i][j][k] += lpack.eta * lpack.ssjr_grad[i][j][k] / sqrt(lpack.ssjr_gradsum[i][j][k] + epsilon);
#ifdef LIMIT_VALUE
	if(lpack.psf.ssjr[i][j][k] > 0){
	  lpack.psf.ssjr[i][j][k] = 0;
	}
#endif
	//cout<<lpack.ssjr_gradsum[i][j][k]<<","<<lpack.ssjr_grad[i][j][k]<<","<<lpack.psf.ssjr[i][j][k]<<","<<i<<","<<j<<","<<k<<endl;

	lpack.ssjr_grad[i][j][k] = 0;
      }
    }
  }

  for(int i=0;i<2;++i){
    for(int j=0;j<bp_max;++j){
      for(int k=0;k<pe_max;++k){
	lpack.bpjr_gradsum[i][j][k] += lpack.bpjr_grad[i][j][k] * lpack.bpjr_grad[i][j][k];
	lpack.bpjr_gradsum[i][j][k] = 0.9999 * lpack.bpjr_gradsum[i][j][k];
	lpack.psf.bpjr[i][j][k] += lpack.eta * lpack.bpjr_grad[i][j][k] / sqrt(lpack.bpjr_gradsum[i][j][k] + epsilon);
#ifdef LIMIT_VALUE
	if(lpack.psf.bpjr[i][j][k]<0){
	  lpack.psf.bpjr[i][j][k]=0;
	}
#endif
	lpack.bpjr_grad[i][j][k] = 0;
	
      }
    }
  }

  for(int i=0;i<2;++i){
    for(int j=0;j<bs_max;++j){
      for(int k=0;k<se_max;++k){
	
	lpack.bsjr_gradsum[i][j][k] += lpack.bsjr_grad[i][j][k] * lpack.bsjr_grad[i][j][k];
	lpack.bsjr_gradsum[i][j][k] = 0.9999 * lpack.bsjr_gradsum[i][j][k];
	lpack.psf.bsjr[i][j][k] += lpack.eta * lpack.bsjr_grad[i][j][k] / sqrt(lpack.bsjr_gradsum[i][j][k] + epsilon);
#ifdef LIMIT_VALUE
	if(lpack.psf.bsjr[i][j][k] < 0){
	  lpack.psf.bsjr[i][j][k] = 0;
	}
#endif
	lpack.bsjr_grad[i][j][k] = 0;
      }
    }
  }

  
  //mirror();
  if(isSave){
    // 評価関数の書き込み
    eval::saveeval("evaltest", lpack.psf);
  }
}

void learner::readTeacher(string fname, Position &pos){
  ifstream ifs(fname.c_str());
  string temp;
  string token;
  uint64_t hand;
  uint64_t myply;
  uint64_t eply[4];
  uint64_t cboard;
  uint64_t suitlock;
  int ptype;
  int prank;
  bool kakumei = false;
  mover mv;
  string gamestr;
  while(getline(ifs,temp)){
    stringstream ss(temp);
    ss >> token;
    if(token == "hand"){
      ss >> hand;
    }else if(token=="gamestr"){
      ss >> gamestr;
    }else if(token == "myply"){
      ss >> myply;
    }else if(token =="eply"){
      int id;
      ss >> id;
      ss >> eply[id];
    }else if(token == "kakumei"){
      int buff;
      ss >> buff;
      kakumei = buff ?  1 : 0;
    }else if(token == "cboard"){
      ss >> cboard;
    }else if(token == "suitlock"){
      ss >> suitlock;
    }else if(token == "type"){
      ss >> ptype;
    }else if(token == "rank"){
      ss >> prank;
    }else if(token == "movtype"){
      int id;
      ss >> id;
      mv.type = (handtype)id;
    }else if(token=="movrank"){
      ss >> mv.rank;
    }else if(token=="movjk"){
      ss >> mv.usejk;
    }else if(token=="movc"){
      ss >> mv.cards;
    }else if(token == "eop"){
      // end of positon
      pos.setPos(hand, myply, eply, kakumei, cboard, suitlock, ptype, prank);
      
#ifdef LEARN_DEBUG
      cout <<"add teacher"<<endl;
      cout<<pos<<endl;
      cout<<mv<<endl;
#endif
      if(!add_teacher(pos,mv)){
	cout<<"game log = "<<gamestr<<endl;
      }
      //cout<<"learn file done"<<endl;
    }
  }
}
