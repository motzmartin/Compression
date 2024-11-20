#include "Quantization.h"

const unsigned char quantizationMatrix[3][8][8] =
{
    {
        { 32, 23, 21, 32, 49, 81, 103, 123 },
        { 25, 25, 29, 39, 53, 117, 121, 111 },
        { 29, 27, 32, 49, 81, 115, 139, 113 },
        { 29, 35, 44, 58, 103, 175, 161, 125 },
        { 37, 44, 75, 113, 137, 219, 207, 155 },
        { 49, 71, 111, 129, 163, 209, 227, 185 },
        { 99, 129, 157, 175, 207, 243, 241, 203 },
        { 145, 185, 191, 197, 225, 201, 207, 199 }
    },
    {
        { 16, 11, 10, 16, 24, 40, 51, 61 },
        { 12, 12, 14, 19, 26, 58, 60, 55 },
        { 14, 13, 16, 24, 40, 57, 69, 56 },
        { 14, 17, 22, 29, 51, 87, 80, 62 },
        { 18, 22, 37, 56, 68, 109, 103, 77 },
        { 24, 35, 55, 64, 81, 104, 113, 92 },
        { 49, 64, 78, 87, 103, 121, 120, 101 },
        { 72, 92, 95, 98, 112, 100, 103, 99 }
    },
    {
        { 8, 6, 5, 8, 12, 20, 26, 31 },
        { 6, 6, 7, 10, 13, 29, 30, 28 },
        { 7, 7, 8, 12, 20, 29, 35, 28 },
        { 7, 9, 11, 15, 26, 44, 40, 31 },
        { 9, 11, 19, 28, 34, 55, 52, 39 },
        { 12, 18, 28, 32, 41, 52, 57, 46 },
        { 25, 32, 39, 44, 52, 61, 60, 51 },
        { 36, 46, 48, 49, 56, 50, 52, 50 }
    }
};

void QUANTIZATION_Quantize(int16_t** dctBlock, int16_t** quantizedBlock, int quality)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            quantizedBlock[i][j] = dctBlock[i][j] / quantizationMatrix[quality][i][j];
        }
    }
}

void QUANTIZATION_Dequantize(int16_t** quantizedBlock, int16_t** dequantizedBlock, int quality)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            dequantizedBlock[i][j] = quantizedBlock[i][j] * quantizationMatrix[quality][i][j];
        }
    }
}