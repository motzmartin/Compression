#define _USE_MATH_DEFINES

#include "DCT2.h"

#include <math.h>

void DCT2_Transform(unsigned char** block, int16_t** dctBlock)
{
    for (int u = 0; u < 8; u++)
    {
        for (int v = 0; v < 8; v++)
        {
            double sum = 0.0;

            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    double shiftedValue = (double)block[x][y] - 128.0;

                    sum += shiftedValue *
                        cos((2.0 * (double)x + 1.0) * (double)u * M_PI / (double)16) *
                        cos((2.0 * (double)y + 1.0) * (double)v * M_PI / (double)16);
                }
            }

            double Cu = (u == 0) ? 1.0 / sqrt(2.0) : 1.0;
            double Cv = (v == 0) ? 1.0 / sqrt(2.0) : 1.0;

            dctBlock[u][v] = (int16_t)(0.25 * Cu * Cv * sum);
        }
    }
}

void DCT2_Invert(int16_t** dctBlock, unsigned char** outputBlock)
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            double sum = 0.0;

            for (int u = 0; u < 8; u++)
            {
                for (int v = 0; v < 8; v++)
                {
                    double Cu = (u == 0) ? 1.0 / sqrt(2.0) : 1.0;
                    double Cv = (v == 0) ? 1.0 / sqrt(2.0) : 1.0;

                    double dctCoeff = (double)dctBlock[u][v];
                    double cosX = cos((2.0 * (double)x + 1.0) * (double)u * M_PI / (double)16);
                    double cosY = cos((2.0 * (double)y + 1.0) * (double)v * M_PI / (double)16);

                    sum += Cu * Cv * dctCoeff * cosX * cosY;
                }
            }

            int pixelValue = (int)round(0.25 * sum + 128.0);

            outputBlock[x][y] = (unsigned char)(pixelValue < 0 ? 0 : (pixelValue > 255 ? 255 : pixelValue));
        }
    }
}