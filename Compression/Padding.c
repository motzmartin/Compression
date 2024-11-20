#include "Padding.h"

#include <stdlib.h>

unsigned char* PADDING_Add(unsigned char* image, int width, int height, int* newWidth, int* newHeight)
{
    *newWidth = width % 16 != 0 ? width + 16 - width % 16 : width;
    *newHeight = height % 16 != 0 ? height + 16 - height % 16 : height;

    unsigned char* newImage = malloc(*newWidth * *newHeight * 3 * sizeof(unsigned char));
    if (!newImage) return NULL;

    for (int i = 0; i < *newHeight; i++)
    {
        for (int j = 0; j < *newWidth; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                if (j < width && i < height)
                {
                    newImage[(i * *newWidth + j) * 3 + k] = image[(i * width + j) * 3 + k];
                }
                else
                {
                    newImage[(i * *newWidth + j) * 3 + k] = 0;
                }
            }
        }
    }

    return newImage;
}

unsigned char* PADDING_Remove(unsigned char* image, int width, int originalWidth, int originalHeight)
{
    unsigned char* newImage = malloc(originalWidth * originalHeight * 3 * sizeof(unsigned char));
    if (!newImage) return NULL;

    for (int i = 0; i < originalHeight; i++)
    {
        for (int j = 0; j < originalWidth; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                newImage[(i * originalWidth + j) * 3 + k] = image[(i * width + j) * 3 + k];
            }
        }
    }

    return newImage;
}