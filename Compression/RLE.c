#include "RLE.h"

unsigned char RLE_Compress(int16_t* block, int16_t* output)
{
    int outIndex = 0;
    int zeroCount = 0;

    for (int i = 0; i < 64; i++)
    {
        if (block[i] == 0)
        {
            zeroCount++;
        }
        else
        {
            output[outIndex++] = zeroCount;
            output[outIndex++] = block[i];
            zeroCount = 0;
        }
    }

    if (zeroCount > 0)
    {
        output[outIndex++] = zeroCount;
    }

    return outIndex;
}

void RLE_Decompress(int16_t* compressedData, unsigned char compressedSize, int16_t* output)
{
    int outIndex = 0;

    for (int i = 0; i < compressedSize; i += 2)
    {
        int zeroCount = compressedData[i];
        int value = compressedData[i + 1];

        for (int j = 0; j < zeroCount; j++)
        {
            output[outIndex++] = 0;
        }

        if (outIndex < 64)
        {
            output[outIndex++] = value;
        }
    }
}