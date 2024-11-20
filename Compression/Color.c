#include "Color.h"

void COLOR_PixelRGBToYCrCb(unsigned char r,
    unsigned char g,
    unsigned char b,
    unsigned char* y,
    unsigned char* cr,
    unsigned char* cb)
{
    *y = (unsigned char)(0.299 * (double)r + 0.587 * (double)g + 0.114 * (double)b);
    *cr = (unsigned char)(((double)r - (double)*y) * 0.713 + 128.0);
    *cb = (unsigned char)(((double)b - (double)*y) * 0.564 + 128.0);
}

void COLOR_ImageRGBToYCrCb(unsigned char* pixels,
    int width,
    int height,
    unsigned char* yPlane,
    unsigned char* crPlane,
    unsigned char* cbPlane)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * 3;
            unsigned char r = pixels[index];
            unsigned char g = pixels[index + 1];
            unsigned char b = pixels[index + 2];

            COLOR_PixelRGBToYCrCb(r, g, b, &yPlane[y * width + x], &crPlane[y * width + x], &cbPlane[y * width + x]);
        }
    }
}

void COLOR_PixelYCrCbToRGB(unsigned char y,
    unsigned char cr,
    unsigned char cb,
    unsigned char* r,
    unsigned char* g,
    unsigned char* b)
{
    int r1 = (int)((double)y + 1.402 * ((double)cr - 128.0));
    int g1 = (int)((double)y - 0.344136 * ((double)cb - 128.0) - 0.714136 * ((double)cr - 128.0));
    int b1 = (int)((double)y + 1.772 * ((double)cb - 128.0));

    *r = (unsigned char)(r1 < 0 ? 0 : (r1 > 255 ? 255 : r1));
    *g = (unsigned char)(g1 < 0 ? 0 : (g1 > 255 ? 255 : g1));
    *b = (unsigned char)(b1 < 0 ? 0 : (b1 > 255 ? 255 : b1));
}

void COLOR_ImageYCrCbToRGB(unsigned char* yPlane,
    unsigned char* crPlane,
    unsigned char* cbPlane,
    int width,
    int height,
    unsigned char* pixels)
{
    int index = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int yIndex = i * width + j;

            unsigned char r, g, b;
            COLOR_PixelYCrCbToRGB(yPlane[yIndex], crPlane[yIndex], cbPlane[yIndex], &r, &g, &b);

            pixels[index++] = r;
            pixels[index++] = g;
            pixels[index++] = b;
        }
    }
}