#include "movepick.h"
#include "position.h"

vector< vector< vector< pairmask > > >movepick::pairmasks;

void movepick::initpairmasks(){
  // movepick::pairmasksの初期化。unorderedmapにしたほうが良かったかも分らん
  vector< vector<uint64_t > > vecbits = {{1,2,4,8},{3,5,9,6,10,12},{7,11,13,14},{15}};
  vector< vector< vector<uint64_t > > > vecjkbits = { {{},{},{},{}}, {{1,2},{1,4},{1,8},{2,4},{2,8},{4,8}}, {{3,5,6}, {3,9,10}, {5,9,12}, {6,10,12}}, {{7,11,13,14}}  };
  
  movepick::pairmasks.resize(4);
  for(uint64_t i=0;i<4;++i){
    movepick::pairmasks[i].resize(32);
    for(uint64_t j=0;j<32;++j){
      movepick::pairmasks[i][j].resize(16);
      for(uint64_t k=0;k<16;++k){
	pairmask pm;
	for(int l = 0; l<vecbits[i].size();++l){
	  if(~vecbits[i][l] & k){continue;}
	  if(j<16){
	    if( !(vecbits[i][l] & ~j)){
	      pm.mask += (vecbits[i][l] << (pm.num*4));
	      ++pm.num;
	    }
	  }else	if ( vecbits[i][l] & ~(j-16) ){
	    for(int m=0;m<vecjkbits[i][l].size();++m){
	      if ( !(vecjkbits[i][l][m] & ~(j-16)) ){
		// joker無しで出せる札はjoker込みの合法手に含めない
		pm.mask += (vecbits[i][l] << (pm.num*4));
		++pm.num;
	      }
	    }
	  }
	}
	movepick::pairmasks[i][j][k] = pm; 
      }
    }
  }
}


void movepick::genmove(Position &pos, vector<mover> &mvs){
  mvs.clear();
  // いかなる状況でもpassは合法手である
  mvs.push_back(mover(0,0,false,PASS));
  if(pos.board.type != PASS){
    genmovebody(pos,mvs,pos.board.type);
  }else{
    genmovebody(pos,mvs,PAIR1);
    genmovebody(pos,mvs,PAIR2);
    genmovebody(pos,mvs,PAIR3);
    genmovebody(pos,mvs,PAIR4);
    // genmovebody(pos,mvs,PAIR5); // 5枚ペアは原理的に返せない
    genmovebody(pos,mvs,STAIR3);
    genmovebody(pos,mvs,STAIR4);
    genmovebody(pos,mvs,STAIR5);
    genmovebody(pos,mvs,STAIR6);
  }

}

void movepick::genmovebody(Position &pos, vector<mover> &mvs, handtype type){
  // jokerの単品出し。手がpair1の場合常にlegalとなる
  if(type == PAIR1){
    if (pos.getMyhand() & CARD_JOKER){
      // 少々乱暴だが8切りと同列扱い
      mvs.push_back(mover(CARD_JOKER, 6, true, PAIR1));
    }
  }

  // s3の単品だし。boardがjoker単体の時に常にlegalとなる
  if(type == PAIR1 && (pos.board.cards & CARD_JOKER)){
    if (pos.getMyhand() & CARD_SPADE3){
      // 少々乱暴だが8切りと同列扱い
      mvs.push_back(mover(CARD_SPADE3, 6, false, PAIR1));
    }
  }

  int plyrank = pos.board.rank;
  if(pos.board.rank == 0 && pos.kakumei){
    plyrank = 14;
  }
  // ペアの処理。pair5はuecルールでは絶対に返さないのでスキップで良い
  if(type > PASS && type < PAIR5){
    pairmask pm;
    const int rankitr = (pos.kakumei ? -1 : 1);
    for(int i = plyrank+rankitr; ; i += rankitr){
      if((i<1 && rankitr < 0) || ( i > 13 && rankitr > 0)){break;} // スートがはみ出たら脱出
      uint64_t handbit = (pos.getMyhand() >> (i*4)) & 15;
      //cout<<"legal pair rank "<<i<<" "<<movepick::pairmasks[type - PAIR1][handbit][pos.board.suitlock]<<endl;
      decodepairmask(movepick::pairmasks[type - PAIR1][handbit][pos.board.suitlock], mvs,i,false,type); // pairmaskのデコード

      if(pos.getMyhand() & CARD_JOKER){
	//cout<<"add jokermove"<<endl;
	handbit += 16;
	decodepairmask(movepick::pairmasks[type - PAIR1][handbit][pos.board.suitlock], mvs,i,true,type); // pairmaskのデコード
      }
      

    }
  }

  // 階段の処理
  const uint64_t kaidanbit[4] = {273,4369,69905,1118481};
  if(type > PAIR5){
    const int rankitr = (pos.kakumei ? -1 : 1);
    mover mv;
    mv.type = type;
    for(int j=0;j<4;++j){
      if ( ~(uint64_t(1) << j) & pos.board.suitlock ){
	continue;
      }
      for(int i = plyrank+rankitr; ; i += rankitr){
	if((i<1 && rankitr < 0) || ( i > 13 && rankitr > 0)){break;} // スートがはみ出たら脱出	
	if( (pos.myhandkaidan[0][type-STAIR3] >> (i*4+j)) & 1){
	  mv.usejk = false;
	  mv.rank = i;
	  mv.cards = (kaidanbit[type-STAIR3] << (i*4+j));
	  mvs.push_back(mv);
	}
	
	if(pos.getMyhand() & CARD_JOKER){
	  if( (pos.myhandkaidan[1][type-STAIR3] >> (i*4+j)) & 1){
	    mv.usejk = true;
	    mv.rank = i;
	    mv.cards = (kaidanbit[type-STAIR3] << (i*4+j));
	    mvs.push_back(mv);
	  }
	}
      }
    }
  }
}

// pairmask形式のデータをmoverに変換する（原理的には不要なのだが可読性大事）
void movepick::decodepairmask(const pairmask &pm, vector<mover> &mvs, int rank, bool usejk, handtype type){
  mover mv;
  mv.rank = rank;
  mv.type = type;
  mv.usejk = usejk;
  for(int i=0;i<pm.num;++i){
    mv.cards = (15 & (pm.mask >> (4*i))) << (4*rank);
    mvs.push_back(mv);
  }
}

std::ostream &operator<<(std::ostream &os, const mover &mv){
  os<<"mover(rank,usejk,type) : "<<mv.rank<<","<<mv.usejk<<","<<mv.type<<endl;
  visbit(os,mv.cards,false);
  return os;
}

std::ostream &operator<<(std::ostream &os, const pairmask &pm){
  os<<"pairmask : "<<pm.num<<" legal hands( masks = "<<pm.mask<<") : ";
  uint64_t bit = 1;
  for(uint64_t i=0;i<pm.num;++i){
    for(uint64_t j=0;j<4;++j){
      if(bit & pm.mask){
	os<<"1";
      }else{
	os<<"0";
      }
      bit <<= 1;
    }
    os<<" / ";
  }
  return os;
}
