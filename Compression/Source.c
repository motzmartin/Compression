#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stb_image.h>
#include <stb_image_write.h>

#include "Compressor.h"
#include "Decompressor.h"

int main(void)
{
    // Image

    int width, height, channels;
    unsigned char* image = stbi_load("image.png", &width, &height, &channels, STBI_rgb);
    if (!image) return 1;

    // Compression

    printf("Compression:\n\n");

    struct Compressed* compressed = COMPRESSOR_Compress(image, width, height, SAMPLING_420, QUALITY_HIGH);
    if (!compressed) return 1;

    printf("\nCompression done!\n\n");

    // Infos

    int originalSize = compressed->width * compressed->height * 3;
    int compressedSize = 0;
    for (int i = 0; i < compressed->numBlocksY; i++)
    {
        compressedSize += compressed->rleSizesY[i];
    }
    for (int i = 0; i < compressed->numBlocksCrCb; i++)
    {
        compressedSize += compressed->rleSizesCr[i] + compressed->rleSizesCb[i];
    }
    printf("Original size: %d Bytes\nCompressed size: %d Bytes\n", originalSize, compressedSize);
    printf("Compression ratio: %d%%\n\n", (int)(((double)compressedSize / (double)originalSize) * 100.0));

    // Decompression

    printf("Decompression:\n\n");

    struct Decompressed* decompressed = DECOMPRESSOR_Decompress(compressed);
    if (!decompressed) return 1;

    COMPRESSOR_Free(compressed);

    printf("\nDecompression done!\n\n");

    if (!stbi_write_png("output.png",
        decompressed->width,
        decompressed->height,
        3,
        decompressed->image,
        decompressed->width * 3)) return 1;

    printf("Image \"output.png\" saved\n");

    DECOMPRESSOR_Free(decompressed);

    return 0;
}