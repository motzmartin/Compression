#include "Blocks.h"

#include <stdlib.h>

unsigned char*** BLOCKS_Init(int numBlocks)
{
    unsigned char*** blocks = malloc(numBlocks * sizeof(unsigned char**));
    if (!blocks) return NULL;

    for (int i = 0; i < numBlocks; i++)
    {
        blocks[i] = malloc(8 * sizeof(unsigned char*));
        if (!blocks[i]) return NULL;

        for (int j = 0; j < 8; j++)
        {
            blocks[i][j] = malloc(8 * sizeof(unsigned char));
            if (!blocks[i][j]) return NULL;
        }
    }

    return blocks;
}

int16_t*** BLOCKS_InitInt16(int numBlocks)
{
    int16_t*** blocks = malloc(numBlocks * sizeof(int16_t**));
    if (!blocks) return NULL;

    for (int i = 0; i < numBlocks; i++)
    {
        blocks[i] = malloc(8 * sizeof(int16_t*));
        if (!blocks[i]) return NULL;

        for (int j = 0; j < 8; j++)
        {
            blocks[i][j] = malloc(8 * sizeof(int16_t));
            if (!blocks[i][j]) return NULL;
        }
    }

    return blocks;
}

void BLOCKS_Create(unsigned char* plane, int width, int height, unsigned char*** blocks)
{
    int blockIndex = 0;

    for (int by = 0; by < height; by += 8)
    {
        for (int bx = 0; bx < width; bx += 8)
        {
            for (int y = 0; y < 8; y++)
            {
                for (int x = 0; x < 8; x++)
                {
                    int imageX = bx + x;
                    int imageY = by + y;

                    blocks[blockIndex][y][x] = plane[imageY * width + imageX];
                }
            }

            blockIndex++;
        }
    }
}

void BLOCKS_ToPlane(unsigned char*** blocks, int width, int height, unsigned char* plane)
{
    int blocksPerRow = width / 8;

    for (int by = 0; by < height / 8; by++)
    {
        for (int bx = 0; bx < width / 8; bx++)
        {
            int blockIndex = by * blocksPerRow + bx;

            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    int planeIndex = (by * 8 + i) * width + (bx * 8 + j);
                    plane[planeIndex] = blocks[blockIndex][i][j];
                }
            }
        }
    }
}

void BLOCKS_Free(unsigned char*** blocks, int num_blocks)
{
    for (int i = 0; i < num_blocks; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            free(blocks[i][j]);
        }

        free(blocks[i]);
    }

    free(blocks);
}

void BLOCKS_FreeInt16(int16_t*** blocks, int num_blocks)
{
    for (int i = 0; i < num_blocks; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            free(blocks[i][j]);
        }

        free(blocks[i]);
    }

    free(blocks);
}