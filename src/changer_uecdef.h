#ifndef _INC_CHANGER_UECDEF_H_
#define _INC_CHANGER_UECDEF_H_
// uecのデフォに実装されてるchange

#include "position.h"

namespace change_uecdef{

  // positionを引数にしておく（正直意味はない）
  void change(Position &pos, int num_of_change, int output[8][15]);
  
    /*
      void change(int out_cards[8][15],int my_cards[8][15],int num_of_change){
      カード交換時のアルゴリズム
      大富豪あるいは富豪が、大貧民あるいは貧民にカードを渡す時のカードを
      カードテーブルmy_cardsと交換枚数num_of_changeに応じて、
      低いほうから選びカードテーブルout_cardsにのせる
      
      int count=0;
      int one_card[8][15];
      
      clearTable(out_cards);
      while(count<num_of_change){
      lowSolo(one_card,my_cards,0);
      cardsDiff(my_cards,one_card);
      cardsOr(out_cards,one_card);
      count++;
      }
      }
    */
}

#endif
