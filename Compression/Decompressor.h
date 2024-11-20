#ifndef __DECOMPRESSOR_H__
#define __DECOMPRESSOR_H__

#include "Compressor.h"

struct Decompressed
{
	int width;
	int height;

	unsigned char* image;
};

struct Decompressed* DECOMPRESSOR_Decompress(struct Compressed* compressed);

void DECOMPRESSOR_Free(struct Decompressed* decompressed);

#endif