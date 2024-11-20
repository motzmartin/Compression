#ifndef __ZIGZAG_H__
#define __ZIGZAG_H__

#include <stdint.h>

extern const unsigned char zigzagOrder[64][2];

void ZIGZAG_Zigzag(int16_t** quantizedBlock, int16_t* output);

void ZIGZAG_ToBlock(int16_t* zigzagBlock, int16_t** outputBlock);

#endif