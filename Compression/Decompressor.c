#include "Decompressor.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "Padding.h"
#include "Color.h"
#include "Sampling.h"
#include "Blocks.h"
#include "DCT2.h"
#include "Quantization.h"
#include "Zigzag.h"
#include "RLE.h"

struct Decompressed* DECOMPRESSOR_Decompress(struct Compressed* compressed)
{
    // Reverse: Entropy coding (RLE)

    printf("Reverse: Entropy coding (RLE)...\n");

    int16_t** zigzagBlocksY = malloc(compressed->numBlocksY * sizeof(int16_t*));
    if (!zigzagBlocksY) return NULL;
    int16_t** zigzagBlocksCr = malloc(compressed->numBlocksCrCb * sizeof(int16_t*));
    if (!zigzagBlocksCr) return NULL;
    int16_t** zigzagBlocksCb = malloc(compressed->numBlocksCrCb * sizeof(int16_t*));
    if (!zigzagBlocksCb) return NULL;

    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        zigzagBlocksY[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksY[i]) return NULL;

        RLE_Decompress(compressed->rleBlocksY[i], compressed->rleSizesY[i], zigzagBlocksY[i]);
    }

    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        zigzagBlocksCr[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksCr[i]) return NULL;
        zigzagBlocksCb[i] = malloc(64 * sizeof(int16_t));
        if (!zigzagBlocksCb[i]) return NULL;

        RLE_Decompress(compressed->rleBlocksCr[i], compressed->rleSizesCr[i], zigzagBlocksCr[i]);
        RLE_Decompress(compressed->rleBlocksCb[i], compressed->rleSizesCb[i], zigzagBlocksCb[i]);
    }

    // Reverse: Entropy coding (zigzag)

    printf("Reverse: Entropy coding (zigzag)...\n");

    int16_t*** quantizedBlocksY = BLOCKS_InitInt16(compressed->numBlocksY);
    if (!quantizedBlocksY) return NULL;
    int16_t*** quantizedBlocksCr = BLOCKS_InitInt16(compressed->numBlocksCrCb);
    if (!quantizedBlocksCr) return NULL;
    int16_t*** quantizedBlocksCb = BLOCKS_InitInt16(compressed->numBlocksCrCb);
    if (!quantizedBlocksCb) return NULL;

    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        ZIGZAG_ToBlock(zigzagBlocksY[i], quantizedBlocksY[i]);
    }

    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        ZIGZAG_ToBlock(zigzagBlocksCr[i], quantizedBlocksCr[i]);
        ZIGZAG_ToBlock(zigzagBlocksCb[i], quantizedBlocksCb[i]);
    }

    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        free(zigzagBlocksY[i]);
    }
    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        free(zigzagBlocksCr[i]);
        free(zigzagBlocksCb[i]);
    }
    free(zigzagBlocksY);
    free(zigzagBlocksCr);
    free(zigzagBlocksCb);

    // Reverse: Quantization (dequantization)

    printf("Reverse: Quantization (dequantization)...\n");

    int16_t*** dctBlocksY = BLOCKS_InitInt16(compressed->numBlocksY);
    if (!dctBlocksY) return NULL;
    int16_t*** dctBlocksCr = BLOCKS_InitInt16(compressed->numBlocksCrCb);
    if (!dctBlocksCr) return NULL;
    int16_t*** dctBlocksCb = BLOCKS_InitInt16(compressed->numBlocksCrCb);
    if (!dctBlocksCb) return NULL;

    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        QUANTIZATION_Dequantize(quantizedBlocksY[i], dctBlocksY[i], compressed->quality);
    }

    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        QUANTIZATION_Dequantize(quantizedBlocksCr[i], dctBlocksCr[i], compressed->quality);
        QUANTIZATION_Dequantize(quantizedBlocksCb[i], dctBlocksCb[i], compressed->quality);
    }

    BLOCKS_FreeInt16(quantizedBlocksY, compressed->numBlocksY);
    BLOCKS_FreeInt16(quantizedBlocksCr, compressed->numBlocksCrCb);
    BLOCKS_FreeInt16(quantizedBlocksCb, compressed->numBlocksCrCb);

    // Reverse: Discrete cosine transform 2

    printf("Reverse: Discrete cosine transform 2\n");

    unsigned char*** blocksY = BLOCKS_Init(compressed->numBlocksY);
    if (!blocksY) return NULL;
    unsigned char*** blocksCr = BLOCKS_Init(compressed->numBlocksCrCb);
    if (!blocksCr) return NULL;
    unsigned char*** blocksCb = BLOCKS_Init(compressed->numBlocksCrCb);
    if (!blocksCb) return NULL;

    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        DCT2_Invert(dctBlocksY[i], blocksY[i]);
    }

    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        DCT2_Invert(dctBlocksCr[i], blocksCr[i]);
        DCT2_Invert(dctBlocksCb[i], blocksCb[i]);
    }

    BLOCKS_FreeInt16(dctBlocksY, compressed->numBlocksY);
    BLOCKS_FreeInt16(dctBlocksCr, compressed->numBlocksCrCb);
    BLOCKS_FreeInt16(dctBlocksCb, compressed->numBlocksCrCb);

    // Reverse: Block splitting

    printf("Reverse: Block splitting...\n");

    unsigned char* yPlane = malloc(compressed->width * compressed->height * sizeof(unsigned char));
    if (!yPlane) return NULL;
    unsigned char* crDownsampled = malloc((compressed->width / compressed->samplingX) * (compressed->height / compressed->samplingY) * sizeof(unsigned char));
    if (!crDownsampled) return NULL;
    unsigned char* cbDownsampled = malloc((compressed->width / compressed->samplingX) * (compressed->height / compressed->samplingY) * sizeof(unsigned char));
    if (!cbDownsampled) return NULL;

    BLOCKS_ToPlane(blocksY, compressed->width, compressed->height, yPlane);
    BLOCKS_ToPlane(blocksCr, compressed->width / compressed->samplingX, compressed->height / compressed->samplingY, crDownsampled);
    BLOCKS_ToPlane(blocksCb, compressed->width / compressed->samplingX, compressed->height / compressed->samplingY, cbDownsampled);

    BLOCKS_Free(blocksY, compressed->numBlocksY);
    BLOCKS_Free(blocksCr, compressed->numBlocksCrCb);
    BLOCKS_Free(blocksCb, compressed->numBlocksCrCb);

    // Reverse: Downsampling (upsampling)

    printf("Reverse: Downsampling (upsampling)...\n");

    unsigned char* crPlane = malloc(compressed->width * compressed->height * sizeof(unsigned char));
    if (!crPlane) return NULL;
    unsigned char* cbPlane = malloc(compressed->width * compressed->height * sizeof(unsigned char));
    if (!cbPlane) return NULL;

    SAMPLING_CrCbUpsample(crDownsampled,
        compressed->width,
        compressed->height,
        compressed->samplingX,
        compressed->samplingY,
        crPlane);
    SAMPLING_CrCbUpsample(cbDownsampled,
        compressed->width,
        compressed->height,
        compressed->samplingX,
        compressed->samplingY,
        cbPlane);

    free(crDownsampled);
    free(cbDownsampled);

    // Reverse: Color space transformation (YCrCrb -> RGB)

    printf("Reverse: Color space transformation (YCrCrb -> RGB)...\n");

    unsigned char* image = malloc(compressed->width * compressed->height * 3 * sizeof(unsigned char));
    if (!image) return NULL;

    COLOR_ImageYCrCbToRGB(yPlane, crPlane, cbPlane, compressed->width, compressed->height, image);

    free(crPlane);
    free(cbPlane);
    free(yPlane);

    // Reverse: Padding

    printf("Reverse: Padding...\n");

    unsigned char* newImage = PADDING_Remove(image,
        compressed->width,
        compressed->originalWidth,
        compressed->originalHeight);
    if (!newImage) return NULL;

    free(image);

    // Struct init

    struct Decompressed* decompressed = malloc(sizeof(struct Decompressed));
    if (!decompressed) return NULL;

    decompressed->width = compressed->originalWidth;
    decompressed->height = compressed->originalHeight;

    decompressed->image = newImage;

    return decompressed;
}

void DECOMPRESSOR_Free(struct Decompressed* decompressed)
{
    free(decompressed->image);

    free(decompressed);
}