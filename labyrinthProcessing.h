#ifndef _LABYRINTH_WAVE_H_
#define _LABYRINTH_WAVE_H_

#include <stdint.h>
#include <stdbool.h>

#define OCCUPIED             (~((uint32_t)0))
#define FREE                 (OCCUPIED - 1)
#define OPTIMIZE_OCCUPIED    (FREE - 1)

typedef struct LabyrinthLabH *LabyrinthLabP;

typedef struct {
    uint32_t x;
    uint32_t y;
} LabyrinthPixelCoord;

typedef struct LabyrinthPixelColor {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} LabyrinthPixelColor;

typedef struct LabyrinthPath {
    uint32_t length;
    LabyrinthPixelCoord *path;
} LabyrinthPath;

typedef bool (*LabyrinthGetPixelCB)(uint32_t x, uint32_t y,
                                    LabyrinthPixelColor *LabyrinthPixelColor);

LabyrinthLabP labyrinthInit(LabyrinthGetPixelCB getPixelCB);
bool labyrinthReadImage(LabyrinthLabP lab);
LabyrinthPath* labyrinthGetPath(LabyrinthLabP lab, LabyrinthPixelCoord startPoint,
                                LabyrinthPixelCoord stopPoint);
LabyrinthPath* labyrinthSmoothPath(LabyrinthPath* path, uint32_t minDistance,
                                         uint32_t deviation);
void labyrinthFree(LabyrinthLabP lab);
void labyrinthPathFree(LabyrinthPath* path);

#endif
