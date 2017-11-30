#ifndef _INC_CARDCONSTANT_H_
#define _INC_CARDCONSTANT_H_
#include "common/common.h"

const uint64_t bit0000 = 0x0000000000000000;
const uint64_t bit0001 = 0x1111111111111111;
const uint64_t bit0010 = 0x2222222222222222;
const uint64_t bit0011 = 0x3333333333333333;
const uint64_t bit0100 = 0x4444444444444444;
const uint64_t bit0101 = 0x5555555555555555;
const uint64_t bit0110 = 0x6666666666666666;
const uint64_t bit0111 = 0x7777777777777777;
const uint64_t bit1000 = 0x8888888888888888;
const uint64_t bit1001 = 0x9999999999999999;
const uint64_t bit1010 = 0xaaaaaaaaaaaaaaaa;
const uint64_t bit1011 = 0xbbbbbbbbbbbbbbbb;
const uint64_t bit1100 = 0xcccccccccccccccc;
const uint64_t bit1101 = 0xdddddddddddddddd;
const uint64_t bit1110 = 0xeeeeeeeeeeeeeeee;
const uint64_t bit1111 = 0xffffffffffffffff;

const uint64_t CARDS_NULL = 0ULL;
const uint64_t CARDS_HORIZON = 1ULL;
const uint64_t CARDS_1SUIT = 15ULL;
const int JOKER_NUM = 60;
const int SPADE3_NUM = 7;
const uint64_t CARD_52 =  (CARDS_HORIZON << (56))- 16;
const uint64_t CARD_JOKER =  CARDS_HORIZON << JOKER_NUM;
const uint64_t CARD_ALL = CARD_52 + CARD_JOKER;
const uint64_t CARD_SPADE3 = CARDS_HORIZON << SPADE3_NUM;
const uint64_t INC_8 = CARDS_1SUIT << (6*4); // card & INC_8 で8を含むか判断

#endif
