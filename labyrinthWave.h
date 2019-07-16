#ifndef _LABYRINTH_WAVE_H_
#define _LABYRINTH_WAVE_H_
#include <stdint.h>
#include <stdbool.h>

#define OCCUPIED (~((lab_t)0))
#define FREE     ((lab_t)0)

typedef uint32_t lab_t;

typedef struct{
    uint32_t x;
    uint32_t y;
} point2D;

typedef struct pathPoint{
    uint32_t x;
    uint32_t y;
    struct pathPoint *nexPoint;
} pathPoint;

bool     initWave(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, point2D start, point2D stop);
pathPoint* findePath(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, point2D start, point2D stop);

#endif
