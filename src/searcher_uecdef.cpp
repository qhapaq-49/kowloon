#include "searcher_uecdef.h"

void search_uecdef::search(Position &pos, int output[8][15]){
  /*
    新しくカードを提出するときの選択ルーチン
    カードテーブルmy_cardsから階段=>ペア=>一枚の順で枚数の多いほうから走査し,
    低いカードからみて、はじめて見つけたものを out_cardsにのせる。
    bit管理などで高速化する意味は...あるのか？（恐らくこれ、超弱い
  */

}
