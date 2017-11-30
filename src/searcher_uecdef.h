#ifndef _INC_SEARCHER_UECDEF_H_
#define _INC_SEARCHER_UECDEF_H_

#include "position.h"

// uecのデフォに実装されてるsearch(leadって呼ばれてる)

namespace search_uecdef{
  void search(Position &pos, int output[8][15]);
}

#endif
