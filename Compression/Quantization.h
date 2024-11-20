#ifndef __QUANTIZATION_H__
#define __QUANTIZATION_H__

#include <stdint.h>

extern const unsigned char quantizationMatrix[3][8][8];

void QUANTIZATION_Quantize(int16_t** dctBlock, int16_t** quantizedBlock, int quality);

void QUANTIZATION_Dequantize(int16_t** quantizedBlock, int16_t** dequantizedBlock, int quality);

#endif