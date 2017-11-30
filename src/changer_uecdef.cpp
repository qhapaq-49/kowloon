#include "changer_uecdef.h"
#include "position.h"
#include "changer_uecdef.h"

void change_uecdef::change(Position &pos, int num_of_change, int output[8][15]){
      int count=0;
      uint64_t temphand = pos.getMyhand();
      // 正直changeを高速化する意味はないが、ビット演算の練習も兼ねてだ。
      for(int i=0;i<num_of_change;++i){
	unsigned long bid = bsf64(temphand);
	temphand -= CARDS_HORIZON << bid;
	//cout<<bid<<endl;
      }
      // どうやら正しく演算で来てる様子。uecのフォーマットに従って値を返すこと
}
