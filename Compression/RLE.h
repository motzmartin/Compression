#ifndef __RLE_H__
#define __RLE_H__

#include <stdint.h>

unsigned char RLE_Compress(int16_t* block, int16_t* output);

void RLE_Decompress(int16_t* compressedData, unsigned char compressedSize, int16_t* output);

#endif