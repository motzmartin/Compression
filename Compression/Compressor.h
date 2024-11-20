#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

#include <stdint.h>

enum SAMPLING
{
    SAMPLING_420,
    SAMPLING_422,
    SAMPLING_444
};

enum QUALITY
{
    QUALITY_LOW,
    QUALITY_MEDIUM,
    QUALITY_HIGH
};

struct Compressed
{
    int width;
    int height;

    int originalWidth;
    int originalHeight;

    int quality;

    int samplingX;
    int samplingY;

    int numBlocksY;
    int numBlocksCrCb;

    int16_t** rleBlocksY;
    int16_t** rleBlocksCr;
    int16_t** rleBlocksCb;

    unsigned char* rleSizesY;
    unsigned char* rleSizesCr;
    unsigned char* rleSizesCb;
};

struct Compressed* COMPRESSOR_Compress(unsigned char* image,
    int imageWidth,
    int imageHeight,
    int sampling,
    int quality);

void COMPRESSOR_Free(struct Compressed* compressed);

#endif