#include "Compressor.h"

#include <stdlib.h>
#include <stdio.h>

#include "Padding.h"
#include "Color.h"
#include "Sampling.h"
#include "Blocks.h"
#include "DCT2.h"
#include "Quantization.h"
#include "Zigzag.h"
#include "RLE.h"

struct Compressed* COMPRESSOR_Compress(unsigned char* image,
    int imageWidth,
    int imageHeight,
    int sampling,
    int quality)
{
    // Sampling values

    int samplingX = 1;
    int samplingY = 1;

    if (sampling == SAMPLING_420)
    {
        samplingX = 2;
        samplingY = 2;
    }
    else if (sampling == SAMPLING_422)
    {
        samplingX = 2;
        samplingY = 1;
    }

    // Padding

    printf("Padding...\n");

    int width, height;

    unsigned char* newImage = PADDING_Add(image, imageWidth, imageHeight, &width, &height);
    if (!newImage) return NULL;

    free(image);

    // Color space transformation (RGB -> YCrCb)

    printf("Color space transformation (RGB -> YCrCb)...\n");

    unsigned char* yPlane = malloc(width * height * sizeof(unsigned char));
    if (!yPlane) return NULL;
    unsigned char* crPlane = malloc(width * height * sizeof(unsigned char));
    if (!crPlane) return NULL;
    unsigned char* cbPlane = malloc(width * height * sizeof(unsigned char));
    if (!cbPlane) return NULL;

    COLOR_ImageRGBToYCrCb(newImage, width, height, yPlane, crPlane, cbPlane);

    // Downsampling

    printf("Downsampling...\n");

    unsigned char* crDownsampled = malloc((width / samplingX) * (height / samplingY) * sizeof(unsigned char));
    if (!crDownsampled) return NULL;
    unsigned char* cbDownsampled = malloc((width / samplingX) * (height / samplingY) * sizeof(unsigned char));
    if (!cbDownsampled) return NULL;

    SAMPLING_CrCbDownsample(crPlane, width, height, samplingX, samplingY, crDownsampled);
    SAMPLING_CrCbDownsample(cbPlane, width, height, samplingX, samplingY, cbDownsampled);

    free(crPlane);
    free(cbPlane);

    // Block splitting

    printf("Block splitting...\n");

    int numBlocksY = (width / 8) * (height / 8);
    int numBlocksCrCb = (width / (samplingX * 8)) * (height / (samplingY * 8));

    unsigned char*** blocksY = BLOCKS_Init(numBlocksY);
    if (!blocksY) return NULL;
    unsigned char*** blocksCr = BLOCKS_Init(numBlocksCrCb);
    if (!blocksCr) return NULL;
    unsigned char*** blocksCb = BLOCKS_Init(numBlocksCrCb);
    if (!blocksCb) return NULL;

    BLOCKS_Create(yPlane, width, height, blocksY);
    BLOCKS_Create(crDownsampled, width / samplingX, height / samplingY, blocksCr);
    BLOCKS_Create(cbDownsampled, width / samplingX, height / samplingY, blocksCb);

    free(yPlane);
    free(crDownsampled);
    free(cbDownsampled);

    // Discrete cosine transform II

    printf("Discrete cosine transform II...\n");

    int16_t*** dctBlocksY = BLOCKS_InitInt16(numBlocksY);
    if (!dctBlocksY) return NULL;
    int16_t*** dctBlocksCr = BLOCKS_InitInt16(numBlocksCrCb);
    if (!dctBlocksCr) return NULL;
    int16_t*** dctBlocksCb = BLOCKS_InitInt16(numBlocksCrCb);
    if (!dctBlocksCb) return NULL;

    for (int i = 0; i < numBlocksY; i++)
    {
        DCT2_Transform(blocksY[i], dctBlocksY[i]);
    }

    for (int i = 0; i < numBlocksCrCb; i++)
    {
        DCT2_Transform(blocksCr[i], dctBlocksCr[i]);
        DCT2_Transform(blocksCb[i], dctBlocksCb[i]);
    }

    BLOCKS_Free(blocksY, numBlocksY);
    BLOCKS_Free(blocksCr, numBlocksCrCb);
    BLOCKS_Free(blocksCb, numBlocksCrCb);

    // Quantization

    printf("Quantization...\n");

    int16_t*** quantizedBlocksY = BLOCKS_InitInt16(numBlocksY);
    if (!quantizedBlocksY) return NULL;
    int16_t*** quantizedBlocksCr = BLOCKS_InitInt16(numBlocksCrCb);
    if (!quantizedBlocksCr) return NULL;
    int16_t*** quantizedBlocksCb = BLOCKS_InitInt16(numBlocksCrCb);
    if (!quantizedBlocksCb) return NULL;

    for (int i = 0; i < numBlocksY; i++)
    {
        QUANTIZATION_Quantize(dctBlocksY[i], quantizedBlocksY[i], quality);
    }

    for (int i = 0; i < numBlocksCrCb; i++)
    {
        QUANTIZATION_Quantize(dctBlocksCr[i], quantizedBlocksCr[i], quality);
        QUANTIZATION_Quantize(dctBlocksCb[i], quantizedBlocksCb[i], quality);
    }

    BLOCKS_FreeInt16(dctBlocksY, numBlocksY);
    BLOCKS_FreeInt16(dctBlocksCr, numBlocksCrCb);
    BLOCKS_FreeInt16(dctBlocksCb, numBlocksCrCb);

    // Entropy coding (zigzag)

    printf("Entropy coding (zigzag)...\n");

    int16_t** zigzagBlocksY = malloc(numBlocksY * sizeof(int16_t*));
    if (!zigzagBlocksY) return NULL;
    int16_t** zigzagBlocksCr = malloc(numBlocksCrCb * sizeof(int16_t*));
    if (!zigzagBlocksCr) return NULL;
    int16_t** zigzagBlocksCb = malloc(numBlocksCrCb * sizeof(int16_t*));
    if (!zigzagBlocksCb) return NULL;

    for (int i = 0; i < numBlocksY; i++)
    {
        zigzagBlocksY[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksY[i]) return NULL;

        ZIGZAG_Zigzag(quantizedBlocksY[i], zigzagBlocksY[i]);
    }

    for (int i = 0; i < numBlocksCrCb; i++)
    {
        zigzagBlocksCr[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksCr[i]) return NULL;
        zigzagBlocksCb[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksCb[i]) return NULL;

        ZIGZAG_Zigzag(quantizedBlocksCr[i], zigzagBlocksCr[i]);
        ZIGZAG_Zigzag(quantizedBlocksCb[i], zigzagBlocksCb[i]);
    }

    BLOCKS_FreeInt16(quantizedBlocksY, numBlocksY);
    BLOCKS_FreeInt16(quantizedBlocksCr, numBlocksCrCb);
    BLOCKS_FreeInt16(quantizedBlocksCb, numBlocksCrCb);

    // Entropy coding (RLE)

    printf("Entropy coding (RLE)...\n");

    int16_t** rleBlocksY = malloc(numBlocksY * sizeof(int16_t*));
    if (!rleBlocksY) return NULL;
    int16_t** rleBlocksCr = malloc(numBlocksCrCb * sizeof(int16_t*));
    if (!rleBlocksCr) return NULL;
    int16_t** rleBlocksCb = malloc(numBlocksCrCb * sizeof(int16_t*));
    if (!rleBlocksCb) return NULL;

    unsigned char* rleSizesY = malloc(numBlocksY * sizeof(unsigned char));
    if (!rleSizesY) return NULL;
    unsigned char* rleSizesCr = malloc(numBlocksCrCb * sizeof(unsigned char));
    if (!rleSizesCr) return NULL;
    unsigned char* rleSizesCb = malloc(numBlocksCrCb * sizeof(unsigned char));
    if (!rleSizesCb) return NULL;

    for (int i = 0; i < numBlocksY; i++)
    {
        rleBlocksY[i] = calloc(128, sizeof(int16_t));
        if (!rleBlocksY[i]) return NULL;

        rleSizesY[i] = RLE_Compress(zigzagBlocksY[i], rleBlocksY[i]);
    }

    for (int i = 0; i < numBlocksCrCb; i++)
    {
        rleBlocksCr[i] = calloc(128, sizeof(int16_t));
        if (!rleBlocksCr[i]) return NULL;
        rleBlocksCb[i] = calloc(128, sizeof(int16_t));
        if (!rleBlocksCb[i]) return NULL;

        rleSizesCr[i] = RLE_Compress(zigzagBlocksCr[i], rleBlocksCr[i]);
        rleSizesCb[i] = RLE_Compress(zigzagBlocksCb[i], rleBlocksCb[i]);
    }

    for (int i = 0; i < numBlocksY; i++)
    {
        free(zigzagBlocksY[i]);
    }
    for (int i = 0; i < numBlocksCrCb; i++)
    {
        free(zigzagBlocksCr[i]);
        free(zigzagBlocksCb[i]);
    }
    free(zigzagBlocksY);
    free(zigzagBlocksCr);
    free(zigzagBlocksCb);

    // Struct init

    struct Compressed* compressed = malloc(sizeof(struct Compressed));
    if (!compressed) return NULL;

    compressed->width = width;
    compressed->height = height;

    compressed->originalWidth = imageWidth;
    compressed->originalHeight = imageHeight;

    compressed->samplingX = samplingX;
    compressed->samplingY = samplingY;

    compressed->quality = quality;

    compressed->numBlocksY = numBlocksY;
    compressed->numBlocksCrCb = numBlocksCrCb;

    compressed->rleBlocksY = rleBlocksY;
    compressed->rleBlocksCr = rleBlocksCr;
    compressed->rleBlocksCb = rleBlocksCb;

    compressed->rleSizesY = rleSizesY;
    compressed->rleSizesCr = rleSizesCr;
    compressed->rleSizesCb = rleSizesCb;

    return compressed;
}

void COMPRESSOR_Free(struct Compressed* compressed)
{
    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        free(compressed->rleBlocksY[i]);
    }
    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        free(compressed->rleBlocksCr[i]);
        free(compressed->rleBlocksCb[i]);
    }
    free(compressed->rleBlocksY);
    free(compressed->rleBlocksCr);
    free(compressed->rleBlocksCb);

    free(compressed->rleSizesY);
    free(compressed->rleSizesCr);
    free(compressed->rleSizesCb);

    free(compressed);
}