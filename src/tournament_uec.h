#ifndef _INC_TOURNAMENT_UEC_H_
#define _INC_TOURNAMENT_UEC_H_

// 自分の勝利回数とか試合回数とかを保存する。
// よーは大会仕様のゲームログの保存
struct Tourstat{
  Tourstat(){
    game_count = 0;
    whole_gameend_flag = false;
  }
  bool whole_gameend_flag;
  int my_playernum;
  int game_count;		 //ゲームの回数を記憶する
};

namespace Tournament{
  extern int main_tournament(int argc,char* argv[]);
}
#endif
