#ifndef __COLOR_H__
#define __COLOR_H__

void COLOR_PixelRGBToYCrCb(unsigned char r,
    unsigned char g,
    unsigned char b,
    unsigned char* y,
    unsigned char* cr,
    unsigned char* cb);

void COLOR_ImageRGBToYCrCb(unsigned char* image,
    int width,
    int height,
    unsigned char* yPlane,
    unsigned char* crPlane,
    unsigned char* cbPlane);

void COLOR_PixelYCrCbToRGB(unsigned char y,
    unsigned char cr,
    unsigned char cb,
    unsigned char* r,
    unsigned char* g,
    unsigned char* b);

void COLOR_ImageYCrCbToRGB(unsigned char* yPlane,
    unsigned char* crPlane,
    unsigned char* cbPlane,
    int width,
    int height,
    unsigned char* pixels);

#endif