#include "stdint.h"
#include "stdbool.h"

#include "optimaizeImage.h"

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
