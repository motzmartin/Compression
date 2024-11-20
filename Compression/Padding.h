#ifndef __PADDING_H__
#define __PADDING_H__

unsigned char* PADDING_Add(unsigned char* image, int width, int height, int* newWidth, int* newHeight);

unsigned char* PADDING_Remove(unsigned char* image, int width, int originalWidth, int originalHeight);

#endif