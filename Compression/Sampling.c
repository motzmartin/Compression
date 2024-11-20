#include "Sampling.h"

void SAMPLING_CrCbDownsample(unsigned char* plane,
    int width,
    int height,
    int samplingX,
    int samplingY,
    unsigned char* downsampledPlane)
{
    for (int i = 0; i < height; i += samplingY)
    {
        for (int j = 0; j < width; j += samplingX)
        {
            int sum = 0;
            int length = 0;

            for (int y = 0; y < samplingY; y++)
            {
                for (int x = 0; x < samplingX; x++)
                {
                    sum += plane[(i + y) * width + (j + x)];
                    length++;
                }
            }

            int downsampledIndex = (i / samplingY) * (width / samplingX) + (j / samplingX);

            downsampledPlane[downsampledIndex] = sum / length;
        }
    }
}

void SAMPLING_CrCbUpsample(unsigned char* downsampledPlane,
    int width,
    int height,
    int samplingX,
    int samplingY,
    unsigned char* upsampledPlane)
{
    int samplingWidth = width / samplingX;
    int samplingHeight = height / samplingY;

    for (int i = 0; i < samplingHeight; i++)
    {
        for (int j = 0; j < samplingWidth; j++)
        {
            int downsampledIndex = i * samplingWidth + j;

            for (int y = 0; y < samplingY; y++)
            {
                for (int x = 0; x < samplingX; x++)
                {
                    int upsampledIndex = (i * samplingY + y) * width + (j * samplingX + x);

                    upsampledPlane[upsampledIndex] = downsampledPlane[downsampledIndex];
                }
            }
        }
    }
}