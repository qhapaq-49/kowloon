#ifndef _INC_MATE_H_
#define _INC_MATE_H_

#include "position.h"
#include "translation.h"

const long int value_mate = 1145141919;

namespace matechecker{
  // posにおいてmvを指すことで勝てるか
  bool isMate(mover &mv, transpos &tp ,Position &pos);
  int getNodompair(uint64_t handpqr, uint64_t enemypqr);
  // その手で盤面が取れるか
  bool isGetboard(mover &mv, Position &pos);
}

#endif
