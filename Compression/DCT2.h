#ifndef __DCT2_H__
#define __DCT2_H__

#include <stdint.h>

void DCT2_Transform(unsigned char** block, int16_t** dctBlock);

void DCT2_Invert(int16_t** dctBlock, unsigned char** outputBlock);

#endif