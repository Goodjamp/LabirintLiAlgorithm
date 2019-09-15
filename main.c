#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string.h>

#include "labyrinthBmpProcessing.h"
#include "labyrinthProcessing.h"

const uint8_t  myImage[] = "C:\\image\\lab_8.bmp";
Point2D startPoint = {.x = 17,  .y = 17};
Point2D stopPoint  = {.x = 367, .y = 370};
//point2D startPoint = {.x = 116,  .y = 84};
//point2D stopPoint  = {.x = 3, .y = 497};
/*
const uint8_t  myImage[] = "C:\\image\\lab_2.bmp";
point2D startPoint = {.x = 12,  .y = 12};
point2D stopPoint  = {.x = 0, .y = 105};

const uint8_t  myImage[] = "C:\\image\\lab_4.bmp";
point2D startPoint = {.x = 2,  .y = 3};
point2D stopPoint  = {.x = 95, .y = 95};

const uint8_t  myImage[] = "C:\\image\\lab_3.bmp";
point2D startPoint = {.x = 0,  .y = 5};
point2D stopPoint  = {.x = 12, .y = 12};
*/

void addTrackToImage(const uint8_t *imagePath, Path *path)
{
    FILE *file;
    const uint8_t nameExtension[] = "_test1846.bmp";
    uint32_t imageFileSize = fileSize(imagePath);
    if(!imageFileSize) {
        printf("Cant Read file\n");
        return;
    }
    uint32_t imageH = getImageHeight(imagePath);
    uint32_t imageW = getImageWidth(imagePath);
    uint32_t imageOfset = getImageOfset(imagePath);
    uint8_t *imageBuff = (uint8_t*)malloc(imageFileSize);
    uint32_t imageWithMem =  (((imageW * sizeof(Color)) % 4) == 0) ?
                               (imageW * sizeof(Color)) :
                               ((((imageW * sizeof(Color)) / 4) + 1) * 4);
    uint8_t (*image)[imageWithMem] = (uint8_t (*)[imageWithMem])(imageBuff + imageOfset);
    if(!(file = fopen(imagePath,"rb")))
    {
        printf("Can't open image file\n");
        return;
    }
    fread(imageBuff, 1, imageFileSize, file);
    fclose(file);
    for(uint32_t k =0; k < path->length; k++) {
        Color *rawImage = (Color*)image[imageH - path->path[k].y - 1];
        rawImage[path->path[k].x].r = 0xFF;
        rawImage[path->path[k].x].g = 0x0;
        rawImage[path->path[k].x].b = 0x0;
    }
    uint8_t *pathName = malloc(strlen(imagePath) + strlen(nameExtension) - 3 + 1);
    memcpy(pathName, imagePath, strlen(imagePath) + 1);
    memcpy(&pathName[strlen(imagePath) - 4], nameExtension, strlen(nameExtension) + 1);
    printf("track file name: %s\n", pathName);
    if(!(file = fopen(pathName,"w")))
    {
        printf("Can't open test.bmp file\n");
        free(pathName);
        free(imageBuff);
        return;
    }
    fwrite(imageBuff, 1, imageFileSize, file);
    fclose(file);
    free(pathName);
    free(imageBuff);
}
/*
void printImage(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW)
{
    printf("PRINT IMAGE \n");
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    printf(" ");
    for(uint32_t k = 0; k < imageH; k++) {
        for(uint32_t i = 0; i < imageW; i++) {
            if(image[k][i] == 0xFFFFFFFF) {
                printf("%s", "#");
            } else if (image[k][i] == 0xEEEEEEEE) {
                printf("%s", ".");
            } else if (image[k][i] == '.') {
                printf("%s", ".");
            } else {
                printf("%s", " ");
            }
        }
        printf("\n");
    }
}
*/
void printWave(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW) {
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    for(uint32_t k = 0; k < imageH; k++) {
        printf("|");
        for(uint32_t i = 0; i < imageW; i++) {
            printf("%3X|", (image[k][i] == OCCUPIED) ? (0xFFF) : (image[k][i]) );
        }
        printf("\n");
    }
}

volatile LabPixel *imageBuff;
volatile uint32_t imageH;
volatile uint32_t imageW;

bool getPixel(uint32_t x, uint32_t y, LabPixel *labPixel)
{
    if(x >= imageW || y >= imageH) {
        return false;
    }
    LabPixel (*image)[imageW] = (LabPixel (*)[imageW])imageBuff;
    *labPixel = image[y][x];
    return true;
}

int main(int argIn, char **argV)
{  
    FILE *imageF;
    const uint8_t *imagePath =  myImage; //argV[1];
    printf("image path: %s \n", imagePath);
    if(!isFileExist(imagePath)) {
        return 0;
    }
    imageH = getImageHeight(imagePath);
    imageW = getImageWidth(imagePath);
    uint32_t imageOfset = getImageOfset(imagePath);
    imageBuff = (LabPixel*)malloc(imageH * imageW * sizeof(LabPixel));
    //uint32_t *imageBuffCopy = (uint32_t*)malloc(imageH * imageW * sizeof(uint32_t));
    //memset(imageBuff, 0, imageH * imageW * sizeof(uint32_t));
    LabPixel (*image)[imageW] = (LabPixel (*)[imageW])imageBuff;
    uint32_t imageWithMem =  (((imageW * sizeof(Color)) % 4) == 0) ?
                               (imageW * sizeof(Color)) :
                               ((((imageW * sizeof(Color)) / 4) + 1) * 4);
    uint8_t *imageRow = malloc(imageWithMem);

    printf("imageH = %d \n", imageH);
    printf("imageW = %d \n", imageW);

    /*Read image*/
    imageF = fopen(imagePath, "rb+");
    fseek(imageF, imageOfset, SEEK_SET);
    for(uint32_t k = imageH; k > 0; k--) {
        LabPixel *color = (LabPixel*)imageRow;
        fread(imageRow, 1, imageWithMem, imageF);
        for(uint32_t i = 0; i < imageW; i++) {
             image[k - 1][i] = color[i];
        }
    }
    free(imageRow);
    LabP lab = labInit(getPixel);
    if(lab == NULL) {
        printf("Cant create labP \n");
        return -1;
    }
    if(!labReadImage(lab)) {
        printf("Cant reade image \n");
        return -1;
    }
    Path* path = labGetPath(lab, startPoint, stopPoint);
    if(path == NULL) {
        printf("Can't finde path \n");
        return 0;
    }
    Path* pathOptimaise = labPathOptimization(path, 10, 6);
    if(pathOptimaise == NULL) {
        printf("Can't optimaixe path\n");
        return 0;
    }
    addTrackToImage(imagePath, pathOptimaise);

    return 0;
}

