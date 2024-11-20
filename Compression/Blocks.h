#ifndef __BLOCKS_H__
#define __BLOCKS_H__

#include <stdint.h>

unsigned char*** BLOCKS_Init(int numBlocks);

int16_t*** BLOCKS_InitInt16(int numBlocks);

void BLOCKS_Create(unsigned char* plane, int width, int height, unsigned char*** blocks);

void BLOCKS_ToPlane(unsigned char*** blocks, int width, int height, unsigned char* plane);

void BLOCKS_Free(unsigned char*** blocks, int num_blocks);

void BLOCKS_FreeInt16(int16_t*** blocks, int num_blocks);

#endif