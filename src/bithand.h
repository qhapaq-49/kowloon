#ifndef _INC_BITHAND_H_
#define _INC_BITHAND_H_
#include "cardconstant.h"

// この辺は大体ohtoさんの記事の写経

namespace bithand{
 
  inline uint64_t convpqr(const uint64_t cards){
    uint64_t a = (cards & bit0101) + ((cards >> 1) & bit0101);
    uint64_t r = a & (a << 2) & bit1000;         // 4ビットあったところを4に配置
    uint64_t r3 = (a << 2) & (a >> 1) & bit0100; // 3ビットあったところを3に配置
    r3 |= a & (a << 1) & bit0100;
    
    // 残りは足すだけ ただし3, 4ビットがすでにあったところにはビットを置かない
    uint64_t r12 = (((a & bit0011) + ((a >> 2) & bit0011))) & bit0011;
    if(r3){
      r |= r3;
      r |= r12 & ~((r3 >> 1) | (r3 >> 2));
    }else{
      r |= r12;
    }
    return r;
  }
  
  inline uint64_t ndMask(const int i){
    return (((1ULL << (i + 1)) - 1ULL) & bit0001) * ((1 << ((i & 3) + 1)) - 1);
  }

  /*
    cardsで作れる階段を列挙する。階段が出せる場合、其の最小の値が1となる
    output[2][4] ... jokerありなし[2] / 3,4,5,6段の階段[4]
  */
  inline void convkaidans(const uint64_t cards, bool jk, uint64_t output[2][4]){
    
    output[0][0] = cards & (cards >> 4) & (cards >> 8); // jokerなしでの3段
    output[0][1] = output[0][0] & (cards >> 12); // jokerなしでの4段
    output[0][2] = output[0][1] & (cards >> 16); // jokerなしでの5段
    output[0][3] = output[0][2] & (cards >> 20); // jokerなしでの6段
        
    if(jk){  
      output[1][0] = ((cards & (cards >> 4)) | (cards & (cards >> 8)) | ((cards >> 4) & (cards >> 8)));
      output[1][1] = ((cards & (cards >> 4) & ( cards >> 8 )) | (cards & (cards >> 4) & (cards >> 12)) | (cards & (cards >> 8) & (cards >> 12)) | ((cards >> 4) & (cards >> 8) & (cards>>12))); 
      output[1][2] =  ((cards & (cards >> 4) & ( cards >> 8 ) & ( cards >> 12 )) |
			      (cards & (cards >> 4) & ( cards >> 8 ) & ( cards >> 16 )) |
			      (cards & (cards >> 4) & ( cards >> 12)  & ( cards >> 16 )) |
		       (cards & ( cards >> 8 ) & ( cards >> 12 ) & ( cards >> 16 )) | ((cards >> 4) & (cards >> 8) & (cards>>12) & (cards>>16)));

      output[1][3] = ((cards & (cards >> 4) & ( cards >> 8 ) & ( cards >> 12 ) & ( cards >> 16 )) |
			      (cards & (cards >> 4) & ( cards >> 8 ) & ( cards >> 12 ) & ( cards >> 20 )) |
			      (cards & (cards >> 4) & ( cards >> 8 ) & ( cards >> 16 ) & ( cards >> 20 )) |
			      (cards & (cards >> 4) & ( cards >> 12 ) & ( cards >> 16 ) & ( cards >> 20 )) |
		      (cards & ( cards >> 8 ) & ( cards >> 12 ) & ( cards >> 16 ) & ( cards >> 20 ))| ((cards >> 4) & (cards >> 8) & (cards>>12) & (cards>>16)& (cards>>20)));
    }else{
      for(int i=0;i<4;++i){
	output[1][i] = 0;
      }
    }
  }
  
  /*
    cardsで作れる階段を列挙する。階段が出せる場合、其の最小の値が1となる
    各階段につき13ビットの情報。スートは取り敢えず見なかったことにする
  */
  
  inline void convkaidandm(uint64_t kaidan[2][4], uint64_t output[2]){

    const bool jk = !(kaidan[1][0] == 0);

    // bitを圧縮する
    output[0] = (bit0001 & (kaidan[0][0] | kaidan[0][0] >> 1 | kaidan[0][0] >> 2 | kaidan[0][0] >> 3)) +
      ((bit0001 & (kaidan[0][1] | kaidan[0][1] >> 1 | kaidan[0][1] >> 2 | kaidan[0][1] >> 3)) << 1) +
      ((bit0001 & (kaidan[0][2] | kaidan[0][2] >> 1 | kaidan[0][2] >> 2 | kaidan[0][2] >> 3)) << 2) +
      ((bit0001 & (kaidan[0][3] | kaidan[0][3] >> 1 | kaidan[0][3] >> 2 | kaidan[0][3] >> 3)) << 3);
    
    if(jk){
      output[1] = (bit0001 & (kaidan[1][0] | kaidan[1][0] >> 1 | kaidan[1][0] >> 2 | kaidan[1][0] >> 3)) +
	((bit0001 & (kaidan[1][1] | kaidan[1][1] >> 1 | kaidan[1][1] >> 2 | kaidan[1][1] >> 3)) << 1) +
	((bit0001 & (kaidan[1][2] | kaidan[1][2] >> 1 | kaidan[1][2] >> 2 | kaidan[1][2] >> 3)) << 2) +
	((bit0001 & (kaidan[1][3] | kaidan[1][3] >> 1 | kaidan[1][3] >> 2 | kaidan[1][3] >> 3)) << 3) - output[0]; // jokerを使わなくていいならそうする

    }else{
      output[1] = 0;
    }
  }

  // 革命でのdomを計算させるためのもの。実装としてクソ
  inline uint64_t reverse(uint64_t bit){
    uint64_t out = 0;
    for(int suit = 0; suit<4;++suit){
      for(int i=1;i<14;++i){
	if(bit & (CARDS_HORIZON << (i*4+suit))){
	  out = out | (CARDS_HORIZON << ((14-i)*4+suit));
	}
      }
    }
    return out;
  }
  
  inline uint64_t convdominance(uint64_t pqr, bool jk){
    uint64_t nd = 0;
    pqr >>= 4;
    while(pqr){ // 無支配ゾーンがまだ広いはず
      int index = bsr64(pqr); // ランク上の方から順に
      nd |= ndMask(index); // このゾーンに対して返せることが確定
      pqr &= ~nd; // もう関係なくなった部分は外す
    }
    // ジョーカーがある場合は1枚分ずらして、全てのシングルを加える
    // 逆転オーダーの場合は+の4枚にフラグがある可能性があるのでマスクする
    if(jk){
      nd <<= 1;
      nd |= bit0001;
    }

    return nd;
  }

  // suitlockによるフィルタリングfor ucb
  inline uint64_t suitlockfilter(uint64_t input, uint64_t suitlock){
    if(suitlock == 0){return input;}
    uint64_t output = 0;
    for(int i=0;i<4;++i){
      if ( (uint64_t(1)<<i) & suitlock){
	output |= input & (bit0001 << i);
      }
    }
    return output;
  }

}

#endif
