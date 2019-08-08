#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "labyrinthProcessing.h"

#define WAVE_SIZE 10000

struct WaveS {
    uint32_t cnt;
    struct {
        uint32_t x;
        uint32_t y;
    } pos[WAVE_SIZE];
};

static pathPoint* addPathPoint(pathPoint* currentPoint, uint32_t x, uint32_t y)
{
    currentPoint->nexPoint = (pathPoint*)malloc(sizeof(pathPoint));
    currentPoint->nexPoint->x = x;
    currentPoint->nexPoint->y = y;
    currentPoint->nexPoint->nexPoint = NULL;
    return currentPoint->nexPoint;
}
/*
bool addPathToTrak(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, point2D start, point2D stop)
{

}
*/

pathPoint* findePath(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, point2D start, point2D stop)
{
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    uint32_t nextDist = image[stop.y][stop.x];
    pathPoint *rootPoint = (pathPoint*)malloc(sizeof(pathPoint));
    pathPoint *tempPoint;
    rootPoint->x = stop.x;
    rootPoint->y = stop.y;
    rootPoint->nexPoint = NULL;
    tempPoint = rootPoint;

    while(true) {
        if(tempPoint->x == start.x
           && tempPoint->y == start.y) {
            break;
        }
        nextDist--;
        if(!nextDist) {
            return NULL;
        }
        if((image[tempPoint->y - 1][tempPoint->x] == nextDist) && (tempPoint->y > 0)) {
            tempPoint = addPathPoint(tempPoint, tempPoint->x, tempPoint->y - 1);
            continue;
        };
        /**BOTTOM PIXEL**/
        if((image[tempPoint->y + 1][tempPoint->x] == nextDist) && (tempPoint->y < (imageH - 1))) {
            tempPoint = addPathPoint(tempPoint, tempPoint->x, tempPoint->y + 1);
            continue;
        };
        /**LEFT PIXEL**/
        if((image[tempPoint->y][tempPoint->x - 1] == nextDist) && (tempPoint->x > 0)) {
            tempPoint = addPathPoint(tempPoint, tempPoint->x - 1, tempPoint->y);
            continue;
        };
        /**RIGHT PIXEL**/
        if((image[tempPoint->y][tempPoint->x + 1] == nextDist) && (tempPoint->x < (imageW - 1))) {
            tempPoint = addPathPoint(tempPoint, tempPoint->x + 1, tempPoint->y);
            continue;
        };
    }
    return rootPoint;
}

bool initWave(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, point2D start, point2D stop)
{
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    struct WaveS wave, waveTemp;
    image[start.y][start.x] = 1;
    wave.cnt = 1;
    wave.pos[0].x = start.x;
    wave.pos[0].y = start.y;

    while(true) {
        waveTemp.cnt = 0;
        for(uint32_t k = 0; k < wave.cnt; k++) {
            uint32_t newVal = image[wave.pos[k].y][wave.pos[k].x] + 1;
            /**UP PIXEL**/
            if((image[wave.pos[k].y - 1][wave.pos[k].x] == 0) && (wave.pos[k].y > 0)) {
                image[wave.pos[k].y - 1][wave.pos[k].x] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y - 1;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                waveTemp.cnt++;
            };
            /**DOWN PIXEL**/
            if((image[wave.pos[k].y + 1][wave.pos[k].x] == 0) && (wave.pos[k].y < (imageH - 1))) {
                image[wave.pos[k].y + 1][wave.pos[k].x] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y + 1;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x;
                waveTemp.cnt++;
            };
            /**LEFT PIXEL**/
            if((image[wave.pos[k].y][wave.pos[k].x - 1] == 0)&& (wave.pos[k].x > 0)) {
                image[wave.pos[k].y ][wave.pos[k].x - 1] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x - 1;
                waveTemp.cnt++;
            };
            /**RIGHT PIXEL**/
            if((image[wave.pos[k].y][wave.pos[k].x + 1] == 0) && (wave.pos[k].x < (imageW - 1))) {
                image[wave.pos[k].y ][wave.pos[k].x + 1] = newVal;
                waveTemp.pos[waveTemp.cnt].y = wave.pos[k].y;
                waveTemp.pos[waveTemp.cnt].x = wave.pos[k].x + 1;
                waveTemp.cnt++;
            };
        }
        wave = waveTemp;
        if(wave.cnt == 0) {
            break;
        }
    }
    return true;
}

void optimazeImage(uint32_t *imageBuff, uint32_t height, uint32_t width)
{
    uint32_t (*image)[width] = (uint8_t (*)[])imageBuff;
    uint16_t imageSize = height * width;
    struct {
        uint32_t x;
        uint32_t y;
    } busyPixelList1[imageSize],
      busyPixelList2[imageSize],
      *currentList,
      *newList;
    currentList = busyPixelList2;
    uint32_t busyCnt = 0;
    /*Get busy pixels list*/
    for(uint32_t kY = 0; kY < height; kY++) {
        for(uint32_t kX = 0; kX < width; kX++) {
            if(image[kY][kX] == 0) {
                continue;
            }
            busyPixelList1[busyCnt].x = kX;
            busyPixelList1[busyCnt].y = kY;
            busyCnt++;

        }
    }
    // increase borders of labirint up to one pixel
    while(busyCnt) {
        uint32_t tempCnt;
        tempCnt = 0;
        if(currentList == busyPixelList1) {
            currentList = busyPixelList2;
            newList     = busyPixelList1;
        } else {
            currentList = busyPixelList1;
            newList     = busyPixelList2;
        }

        for(uint32_t k = 0; k < busyCnt; k++) {

            if(currentList[k].y >= 2 &&
               currentList[k].x >= 1 &&
               currentList[k].x <= (width - 2)) {
                if(image[currentList[k].y - 1][currentList[k].x] == 0 &&
                   image[currentList[k].y - 2][currentList[k].x - 1] == 0 &&
                   image[currentList[k].y - 2][currentList[k].x] == 0 &&
                   image[currentList[k].y - 2][currentList[k].x + 1] == 0) {
                    image[currentList[k].y - 1][currentList[k].x] = OPTIMAIZ_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x;
                    newList[tempCnt].y = currentList[k].y - 1;
                    tempCnt++;
                }
            }
            if(currentList[k].y <= (height - 3) &&
               currentList[k].x >= 1            &&
               currentList[k].x <= (width - 2)) {
                if(image[currentList[k].y + 1][currentList[k].x] == 0 &&
                   image[currentList[k].y + 2][currentList[k].x - 1] == 0 &&
                   image[currentList[k].y + 2][currentList[k].x] == 0 &&
                   image[currentList[k].y + 2][currentList[k].x + 1] == 0) {
                    image[currentList[k].y + 1][currentList[k].x] = OPTIMAIZ_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x;
                    newList[tempCnt].y = currentList[k].y + 1;
                    tempCnt++;
                }
            }

            if(currentList[k].x >= 2 &&
               currentList[k].y >= 1 &&
               currentList[k].y <= (height - 2)) {
                if(image[currentList[k].y][currentList[k].x - 1] == 0 &&
                   image[currentList[k].y - 1][currentList[k].x - 2] == 0 &&
                   image[currentList[k].y][currentList[k].x - 2] == 0 &&
                   image[currentList[k].y + 1][currentList[k].x - 2] == 0) {
                    image[currentList[k].y][currentList[k].x - 1] = OPTIMAIZ_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x - 1;
                    newList[tempCnt].y = currentList[k].y;
                    tempCnt++;
                }
            }

            if(currentList[k].x <= (width - 3) &&
               currentList[k].y >= 1           &&
               currentList[k].y <= (height - 2)) {
                if(image[currentList[k].y][currentList[k].x + 1] == 0 &&
                   image[currentList[k].y - 1][currentList[k].x + 2] == 0 &&
                   image[currentList[k].y][currentList[k].x + 2] == 0 &&
                   image[currentList[k].y + 1][currentList[k].x + 2] == 0) {
                    image[currentList[k].y][currentList[k].x + 1] = OPTIMAIZ_OCCUPIED;
                    newList[tempCnt].x = currentList[k].x + 1;
                    newList[tempCnt].y = currentList[k].y;
                    tempCnt++;
                }
            }

        }
        busyCnt = tempCnt;
    }

}
