#ifndef _LABYRINTH_WAVE_H_
#define _LABYRINTH_WAVE_H_
#include <stdint.h>
#include <stdbool.h>

#define OCCUPIED          (~((lab_t)0))
#define FREE              (OCCUPIED - 1)
#define OPTIMIZE_OCCUPIED (FREE - 1)

typedef uint32_t lab_t;

typedef struct LabH *LabP;

typedef struct{
    uint32_t x;
    uint32_t y;
} Point2D;

typedef struct LabPixel{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} LabPixel;

typedef struct Path{
    uint32_t length;
    Point2D *path;
} Path;

typedef bool (*GetPixelCB)(uint32_t x, uint32_t y, LabPixel *labPixel);

LabP  labInit(GetPixelCB getPixelCB);
bool  labReadImage(LabP lab);
Path* labGetPath(LabP lab, Point2D startPoint, Point2D stopPoint);
Path* labPathOptimization(Path* path, uint32_t minDistance, uint32_t deviation);
void  ladFree(LabP lab);
void  labPathFree(Path* path);

#endif
