#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "labyrinthProcessing.h"

#define WAVE_SIZE 100000

typedef struct LabyrinthLabH {
    uint32_t *imageBuff;
    uint32_t *persistentImageBuff;
    uint32_t imageH;
    uint32_t imageW;
    bool isInit;
    LabyrinthGetPixelCB getPixel;
} LabH_;

struct WaveS {
    uint32_t cnt;
    struct {
        uint32_t x;
        uint32_t y;
    } pos[WAVE_SIZE];
};

static void printImagePart(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, uint32_t x, uint32_t y)
{
    printf("PRINT IMAGE \n");
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    printf(" ");
    for (uint32_t k = y; k < imageH; k++) {
        for (uint32_t i = x; i < imageW; i++) {
            if (image[k][i] == OCCUPIED) {
                printf("%s", "#");
            } else if (image[k][i] == OPTIMIZE_OCCUPIED) {
                printf("%s", ".");
            } else if (image[k][i] == FREE) {
                printf("%s", " ");
            } else {
                printf("%s", "0");
            }
        }
        printf("\n");
    }
}

static LabyrinthPath* labFindePath(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, LabyrinthPixelCoord start, LabyrinthPixelCoord stop)
{
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    uint32_t nextDist = image[stop.y][stop.x];
    uint32_t nextDistPrev;
    uint32_t pathLength = nextDist + 1;

    if (nextDist == OCCUPIED ||
        nextDist == OPTIMIZE_OCCUPIED ||
        nextDist == FREE) {
        return NULL;
    }
    if (start.x == stop.x && start.y == stop.y) {
        return NULL;
    }
    LabyrinthPixelCoord *rootPoint = (LabyrinthPixelCoord*)malloc(sizeof(LabyrinthPixelCoord) * (nextDist + 1));
    rootPoint[nextDist].x = stop.x;
    rootPoint[nextDist].y = stop.y;
    while (true) {
        if ((rootPoint[nextDist].x == start.x && rootPoint[nextDist].y == start.y)
            || nextDist == 0) {
            break;
        }
        nextDistPrev = nextDist;
        nextDist--;
        if ((image[rootPoint[nextDistPrev].y - 1][rootPoint[nextDistPrev].x] == nextDist) && (rootPoint[nextDistPrev].y > 0)) {
            rootPoint[nextDist].x = rootPoint[nextDistPrev].x;
            rootPoint[nextDist].y = rootPoint[nextDistPrev].y - 1;
            continue;
        };
        /**BOTTOM PIXEL**/
        if ((image[rootPoint[nextDistPrev].y + 1][rootPoint[nextDistPrev].x] == nextDist) && (rootPoint[nextDistPrev].y < (imageH - 1))) {
            rootPoint[nextDist].x = rootPoint[nextDistPrev].x;
            rootPoint[nextDist].y = rootPoint[nextDistPrev].y + 1;
            continue;
        };
        /**LEFT PIXEL**/
        if ((image[rootPoint[nextDistPrev].y][rootPoint[nextDistPrev].x - 1] == nextDist) && (rootPoint[nextDistPrev].x > 0)) {
            rootPoint[nextDist].x = rootPoint[nextDistPrev].x - 1;
            rootPoint[nextDist].y = rootPoint[nextDistPrev].y;
            continue;
        };
        /**RIGHT PIXEL**/
        if ((image[rootPoint[nextDistPrev].y][rootPoint[nextDistPrev].x + 1] == nextDist) && (rootPoint[nextDistPrev].x < (imageW - 1))) {
            rootPoint[nextDist].x = rootPoint[nextDistPrev].x + 1;
            rootPoint[nextDist].y = rootPoint[nextDistPrev].y;
            continue;
        };
        return NULL;
    }
    LabyrinthPath *rezPath = (LabyrinthPath*)malloc(sizeof(LabyrinthPath));
    rezPath->length = pathLength;
    rezPath->path = rootPoint;
    return rezPath;
}

/*
 * Add path to the neares free space
 */
static bool labExtendPathFromPoint(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, LabyrinthPixelCoord position)
{
    uint32_t *copyImage = (uint32_t*)malloc(imageH * imageW * sizeof(uint32_t));
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])copyImage;
    uint32_t (*imageBuffP)[imageW] = (uint32_t (*)[imageW])imageBuff;
    struct WaveS wave, waveTemp;

    if (imageH <= position.y || imageW <= position.x) {
        return false;
    }
    if (imageBuffP[position.y][position.x] == OCCUPIED) {
        return false;
    }
    if (imageBuffP[position.y][position.x] == FREE) {
        return true;
    }
    memcpy((uint8_t*)copyImage, (uint8_t*)imageBuff, imageH * imageW * sizeof(uint32_t));
    image[position.y][position.x] = 0;
    wave.cnt = 1;
    wave.pos[0].x = position.x;
    wave.pos[0].y = position.y;
    uint32_t rezX = 0,
             rezY = 0;
    bool isContinue = true;
    while (true) {
        waveTemp.cnt = 0;
        for (uint32_t k = 0; k < wave.cnt; k++) {
            uint32_t newVal = image[wave.pos[k].y][wave.pos[k].x] + 1;

            /**UP PIXEL**/
            if (wave.pos[k].y > 0) {
                if (image[wave.pos[k].y - 1][wave.pos[k].x] == OPTIMIZE_OCCUPIED) {
                image[wave.pos[k].y - 1][wave.pos[k].x] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y - 1;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                waveTemp.cnt++;
                } else if (image[wave.pos[k].y - 1][wave.pos[k].x] == FREE){
                    rezX = wave.pos[k].x;
                    rezY = wave.pos[k].y;
                    isContinue = false;
                    break;
                };
            };
            /**DOWN PIXEL**/
            if (wave.pos[k].y < (imageH - 1)){
                if (image[wave.pos[k].y + 1][wave.pos[k].x] == OPTIMIZE_OCCUPIED){
                    image[wave.pos[k].y + 1][wave.pos[k].x] = newVal;
                    waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y + 1;
                    waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                    waveTemp.cnt++;
                } else if (image[wave.pos[k].y + 1][wave.pos[k].x] == FREE) {
                    rezX = wave.pos[k].x;
                    rezY = wave.pos[k].y;
                    isContinue = false;
                    break;
                }
            }
            /**LEFT PIXEL**/
            if (wave.pos[k].x > 0) {
                if (image[wave.pos[k].y][wave.pos[k].x - 1] == OPTIMIZE_OCCUPIED) {
                    image[wave.pos[k].y][wave.pos[k].x - 1] = newVal;
                    waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                    waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x - 1;
                    waveTemp.cnt++;
                } else if (image[wave.pos[k].y][wave.pos[k].x - 1] == FREE) {
                    rezX = wave.pos[k].x;
                    rezY = wave.pos[k].y;
                    isContinue = false;
                    break;
                }
            }
            /**RIGHT PIXEL**/
            if (wave.pos[k].x < (imageW - 1)) {
                if (image[wave.pos[k].y][wave.pos[k].x + 1] == OPTIMIZE_OCCUPIED) {
                    image[wave.pos[k].y][wave.pos[k].x + 1] = newVal;
                    waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                    waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x + 1;
                    waveTemp.cnt++;
                } else if (image[wave.pos[k].y][wave.pos[k].x + 1] == FREE) {
                    rezX = wave.pos[k].x;
                    rezY = wave.pos[k].y;
                    isContinue = false;
                    break;

                }
            }
        }
        if (waveTemp.cnt > 0 && isContinue){
            wave = waveTemp;
        } else if (waveTemp.cnt == 0 && isContinue) {
             free(copyImage);
             return false;
        } else {
            break;
        }
    }
    if (position.x == rezX && position.y == rezY) {
        imageBuffP[position.y][position.x] = FREE;
        free(copyImage);
        return true;
    }
    LabyrinthPath *path = labFindePath(copyImage,
                                       imageH,
                                       imageW,
                                       position,
                                       (LabyrinthPixelCoord){rezX, rezY});
    if (path == NULL) {
        free(copyImage);
        free(path->path);
        free(path);
        return false;
    }
    /*Set path to start point*/
    for (uint32_t k = 0; k < path->length; k++){
        imageBuffP[path->path[k].y][path->path[k].x] = FREE;
    }
    free(copyImage);
    free(path->path);
    free(path);
    return true;
}

static bool labyrinthInitWave(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW,
                              LabyrinthPixelCoord start, LabyrinthPixelCoord stop)
{
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    struct WaveS wave, waveTemp;

    if (imageH <= start.y || imageW <= start.x) {
        return NULL;
    }
    if (imageH <= stop.y || imageW <= stop.x) {
        return NULL;
    }
    if (image[start.y][start.x] != FREE) {
        return NULL;
    }
    image[start.y][start.x] = 0;
    wave.cnt = 1;
    wave.pos[0].x = start.x;
    wave.pos[0].y = start.y;

    while (true) {
        waveTemp.cnt = 0;
        for (uint32_t k = 0; k < wave.cnt; k++) {
            uint32_t newVal = image[wave.pos[k].y][wave.pos[k].x] + 1;
            /**UP PIXEL**/
            if ((image[wave.pos[k].y - 1][wave.pos[k].x] == FREE) && (wave.pos[k].y > 0)) {
                image[wave.pos[k].y - 1][wave.pos[k].x] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y - 1;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                waveTemp.cnt++;
            };
            /**DOWN PIXEL**/
            if ((image[wave.pos[k].y + 1][wave.pos[k].x] == FREE) && (wave.pos[k].y < (imageH - 1))) {
                image[wave.pos[k].y + 1][wave.pos[k].x] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y + 1;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                waveTemp.cnt++;
            };
            /**LEFT PIXEL**/
            if ((image[wave.pos[k].y][wave.pos[k].x - 1] == FREE)&& (wave.pos[k].x > 0)) {
                image[wave.pos[k].y][wave.pos[k].x - 1] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x - 1;
                waveTemp.cnt++;
            };
            /**RIGHT PIXEL**/
            if ((image[wave.pos[k].y][wave.pos[k].x + 1] == FREE) && (wave.pos[k].x < (imageW - 1))) {
                image[wave.pos[k].y][wave.pos[k].x + 1] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x + 1;
                waveTemp.cnt++;
            };
        }
        wave = waveTemp;
        if (wave.cnt == 0) {
            break;
        }
    }
    return true;
}

/*
 * Thickening the walls of the labyrinth up to the moment when remain the
 * pass thickness of one pixel.
 */
static void labyrinthImageWallThickening(uint32_t *imageBuff, uint32_t height,
                                         uint32_t width)
{
    uint32_t (*image)[width] = (uint32_t (*)[])imageBuff;
    uint32_t imageSize = height * width;
    LabyrinthPixelCoord *busyPixelList1 = (LabyrinthPixelCoord*)malloc(sizeof(LabyrinthPixelCoord) * imageSize);
    LabyrinthPixelCoord *busyPixelList2 = (LabyrinthPixelCoord*)malloc(sizeof(LabyrinthPixelCoord) * imageSize);
    LabyrinthPixelCoord *currentList, *newList;
    currentList = busyPixelList2;
    uint32_t busyCnt = 0;

    /*Get busy pixels list*/
    for (uint32_t kY = 0; kY < height; kY++) {
        for (uint32_t kX = 0; kX < width; kX++) {
            if (image[kY][kX] == FREE) {
                continue;
            }
            busyPixelList1[busyCnt].x = kX;
            busyPixelList1[busyCnt].y = kY;
            busyCnt++;

        }
    }
    // increase borders of labirint up to one pixel
    while (busyCnt) {
        uint32_t tempCnt;
        tempCnt = 0;
        if (currentList == busyPixelList1) {
            currentList = busyPixelList2;
            newList = busyPixelList1;
        } else {
            currentList = busyPixelList1;
            newList = busyPixelList2;
        }

        for (uint32_t k = 0; k < busyCnt; k++) {

            if (currentList[k].y >= 2 &&
               currentList[k].x >= 1 &&
               currentList[k].x <= (width - 2)) {
                if (image[currentList[k].y - 1][currentList[k].x] == FREE &&
                   image[currentList[k].y - 2][currentList[k].x - 1] == FREE &&
                   image[currentList[k].y - 2][currentList[k].x] == FREE &&
                   image[currentList[k].y - 2][currentList[k].x + 1] == FREE) {
                    image[currentList[k].y - 1][currentList[k].x] = OPTIMIZE_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x;
                    newList[tempCnt].y = currentList[k].y - 1;
                    tempCnt++;
                }
            }
            if (currentList[k].y <= (height - 3) &&
               currentList[k].x >= 1 &&
               currentList[k].x <= (width - 2)) {
                if (image[currentList[k].y + 1][currentList[k].x] == FREE &&
                   image[currentList[k].y + 2][currentList[k].x - 1] == FREE &&
                   image[currentList[k].y + 2][currentList[k].x] == FREE &&
                   image[currentList[k].y + 2][currentList[k].x + 1] == FREE) {
                    image[currentList[k].y + 1][currentList[k].x] = OPTIMIZE_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x;
                    newList[tempCnt].y = currentList[k].y + 1;
                    tempCnt++;
                }
            }

            if (currentList[k].x >= 2 &&
               currentList[k].y >= 1 &&
               currentList[k].y <= (height - 2)) {
                if (image[currentList[k].y][currentList[k].x - 1] == FREE &&
                   image[currentList[k].y - 1][currentList[k].x - 2] == FREE &&
                   image[currentList[k].y][currentList[k].x - 2] == FREE &&
                   image[currentList[k].y + 1][currentList[k].x - 2] == FREE) {
                    image[currentList[k].y][currentList[k].x - 1] = OPTIMIZE_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x - 1;
                    newList[tempCnt].y = currentList[k].y;
                    tempCnt++;
                }
            }

            if (currentList[k].x <= (width - 3) &&
               currentList[k].y >= 1 &&
               currentList[k].y <= (height - 2)) {
                if (image[currentList[k].y][currentList[k].x + 1] == FREE &&
                   image[currentList[k].y - 1][currentList[k].x + 2] == FREE &&
                   image[currentList[k].y][currentList[k].x + 2] == FREE &&
                   image[currentList[k].y + 1][currentList[k].x + 2] == FREE) {
                    image[currentList[k].y][currentList[k].x + 1] = OPTIMIZE_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x + 1;
                    newList[tempCnt].y = currentList[k].y;
                    tempCnt++;
                }
            }
        }
        busyCnt = tempCnt;
    }
    free(busyPixelList1);
    free(busyPixelList2);
}

bool labyrinthReadImage(LabyrinthLabP lab)
{
    LabyrinthPixelColor labPixel;
    uint32_t imageH = 0;
    uint32_t imageW = 0;
    if (lab->imageBuff != NULL) {
        free(lab->imageBuff);
    }
    if (lab->persistentImageBuff != NULL) {
        free(lab->persistentImageBuff);
    }
    // get image hwidth
    while (lab->getPixel(imageW, 0, &labPixel)) {
        imageW++;
    }
    //get image height
    while (lab->getPixel(0, imageH, &labPixel)) {
        imageH++;
    }
    uint32_t *imageBuff = (uint32_t*)malloc(sizeof(uint32_t) * imageH * imageW);
    uint32_t (*image)[imageW] = (uint32_t (*)[])imageBuff;
    for (uint32_t k = 0; k < imageH; k++) {
        for (uint32_t i = 0; i < imageW; i++) {
            if (!lab->getPixel(i, k, &labPixel)) {
                free(imageBuff);
                return false;
            }
            labPixel.r = 0xFF - labPixel.r;
            labPixel.g = 0xFF - labPixel.g;
            labPixel.b = 0xFF - labPixel.b;
            if ((labPixel.r > 0) || (labPixel.g > 0) || (labPixel.b > 0)) {
                image[k][i] = OCCUPIED;
            } else {
                image[k][i] = FREE;
            }
        }
    }
    for (uint32_t k = 0; k < imageW; k++) {
        image[0][k] = OCCUPIED;
        image[imageH - 1][k] = OCCUPIED;
    }
    for (uint32_t k = 0; k < imageH; k++) {
        image[k][0] = OCCUPIED;
        image[k][imageW - 1] = OCCUPIED;
    }
    lab->imageH = imageH;
    lab->imageW = imageW;
    lab->isInit = true;
    lab->imageBuff = imageBuff;
    lab->persistentImageBuff = (uint32_t*)malloc(sizeof(uint32_t) * imageH * imageW);

    /*
     * Thickening the walls of the labyrinth up to the moment when remain the pass thickness of one pixel.
     */
    labyrinthImageWallThickening(imageBuff, imageH, imageW);
    memcpy((uint8_t*)lab->persistentImageBuff, (uint8_t*)lab->imageBuff, sizeof(uint32_t) * imageH * imageW);
    return true;
}

LabyrinthLabP labyrinthInit(LabyrinthGetPixelCB getPixelCB)
{
    LabyrinthLabP lab = (LabyrinthLabP)malloc(sizeof(LabyrinthLabP));
    lab->imageH = 0;
    lab->imageH = 0;
    lab->isInit = false;
    lab->imageBuff = NULL;
    lab->persistentImageBuff = NULL;
    lab->getPixel = getPixelCB;
    return lab;
}

LabyrinthPath* labyrinthGetPath(LabyrinthLabP lab, LabyrinthPixelCoord startPoint, LabyrinthPixelCoord stopPoint)
{
    memcpy((uint8_t*)lab->imageBuff,
           (uint8_t*)lab->persistentImageBuff,
           sizeof(uint32_t) * lab->imageH * lab->imageW);
    if (!labExtendPathFromPoint(lab->imageBuff, lab->imageH, lab->imageW, startPoint)) {
        return NULL;
    }
    if (!labExtendPathFromPoint(lab->imageBuff, lab->imageH, lab->imageW, stopPoint)) {
        return NULL;
    }
    if (!labyrinthInitWave(lab->imageBuff, lab->imageH, lab->imageW, startPoint, (LabyrinthPixelCoord){0,0})) {
        return NULL;
    }

    return labFindePath(lab->imageBuff, lab->imageH, lab->imageW, startPoint, stopPoint);
}

void labyrinthFree(LabyrinthLabP lab)
{
    if (lab == NULL) {
        return;
    }
    if (lab->imageBuff != NULL) {
        free(lab->imageBuff);
    }
    if (lab->persistentImageBuff != NULL) {
        free(lab->persistentImageBuff);
    }
    free(lab);
}

void labyrinthPathFree(LabyrinthPath* path)
{
    if (path == NULL) {
        return;
    }
    if (path->path != NULL) {
        free(path->path);
    }
    free(path);
}

static inline double labPointToLineDist(LabyrinthPixelCoord point, double A, double B, double C)
{
    double x = (B * ( B * point.x - A * point.y) - A * C) / (A * A + B * B);
    double y = (A * (-1.0 * B * point.x + A * point.y) - B * C) / (A * A + B * B);

    return (point.x - x) * (point.x - x) + (point.y - y) * (point.y - y);
}

static bool labIsInRange(LabyrinthPixelCoord* path, uint32_t size, double quadDL)
{
    double lA = 1.0 * ((double)path[size - 1].y - (double)path[0].y);
    double lB = -1.0 * ((double)path[size - 1].x - (double)path[0].x);
    double lC = -1.0 * lB * (double)path[0].y - lA * (double)path[0].x;

    for (uint32_t k = 0; k < size; k++) {
        if (quadDL < labPointToLineDist(path[k], lA, lB, lC)) {
            return false;
        }
    }
    return true;
}

LabyrinthPath* labyrinthSmoothPath(LabyrinthPath* path, uint32_t minSegmentSize, uint32_t deviation)
{
    LabyrinthPixelCoord *pathPoint = (LabyrinthPixelCoord*)malloc(path->length * sizeof(LabyrinthPixelCoord));
    uint32_t oldPCnt = 0;
    uint32_t newPCnt = 0;
    uint32_t size;
    double squaredDeviation = deviation * deviation;

    if (path == NULL) {
        return NULL;
    }

    pathPoint[newPCnt++] = path->path[oldPCnt];
    minSegmentSize++;
    while ((oldPCnt + 1) < path->length) {
        size = ((path->length - (oldPCnt + 1)) >= minSegmentSize)
               ? (minSegmentSize)
               : (path->length - (oldPCnt + 1));
        while (labIsInRange(&path->path[oldPCnt], size, squaredDeviation)) {
            size++;
            if ((oldPCnt + size) > path->length) {
                break;
            }
        }
        size--;
        oldPCnt += size - 1;
        pathPoint[newPCnt++] = path->path[oldPCnt];
    }

    LabyrinthPath* newPath = (LabyrinthPath*)malloc(sizeof(LabyrinthPath));
    newPath->length = newPCnt;
    newPath->path = pathPoint;
    return newPath;
}


