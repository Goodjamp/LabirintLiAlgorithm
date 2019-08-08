#include <stdio.h>
#include <stdint.h>

#include "bmpDescriptions.h"
#include "labyrinthWave.h"
#include "optimaizeImage.h"


const uint8_t  myImage[] = "C:\\image\\lab_5.bmp";
point2D startPoint = {.x = 2,  .y = 3};
point2D stopPoint  = {.x = 497, .y = 3};
/*
const uint8_t  myImage[] = "C:\\image\\lab_2.bmp";
point2D startPoint = {.x = 12,  .y = 12};
point2D stopPoint  = {.x = 0, .y = 105};

const uint8_t  myImage[] = "C:\\image\\lab_4.bmp";
point2D startPoint = {.x = 2,  .y = 3};
point2D stopPoint  = {.x = 95, .y = 95};

const uint8_t  myImage[] = "C:\\image\\lab_3.bmp";
point2D startPoint = {.x = 0,  .y = 0};
point2D stopPoint  = {.x = 12, .y = 12};
*/

void printImageInfo(const uint8_t *imagePath)
{
    union {
        BmpS file;
        uint8_t buff[sizeof(BmpS)];
    }bmpHeadr;
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return;
    }
    fread(bmpHeadr.buff, 1, sizeof(bmpHeadr), myImage);
    printf("file size = %d \n", bmpHeadr.file.bmpHeadr.fileSize);
    printf("pixels cnt = %d \n", bmpHeadr.file.bmpInfoHeadr.size);
    printf("width = %d \n", bmpHeadr.file.bmpInfoHeadr.width);
    printf("height = %d \n", bmpHeadr.file.bmpInfoHeadr.height);
    printf("imageSize = %d \n", bmpHeadr.file.bmpInfoHeadr.imageSize);
    printf("bitsPerixel = %d \n", bmpHeadr.file.bmpInfoHeadr.bitsPerixel);
    printf("data begin = %d \n", bmpHeadr.file.bmpHeadr.dataOfset);

    fclose(myImage);
}

uint32_t getImageSize(uint8_t *imagePath)
{
    union {
        BmpS file;
        uint8_t buff[sizeof(BmpS)];
    }bmpHeadr;
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return 0;
    }
    fread(bmpHeadr.buff, 1, sizeof(bmpHeadr), myImage);
    fclose(myImage);
    return bmpHeadr.file.bmpInfoHeadr.width
           * bmpHeadr.file.bmpInfoHeadr.height;
}

uint32_t getImageHeight(uint8_t *imagePath)
{
    union {
        BmpS file;
        uint8_t buff[sizeof(BmpS)];
    }bmpHeadr;
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return 0;
    }
    fread(bmpHeadr.buff, 1, sizeof(bmpHeadr), myImage);
    fclose(myImage);
    return bmpHeadr.file.bmpInfoHeadr.height;
}

uint32_t getImageWidth(uint8_t *imagePath)
{
    union {
        BmpS file;
        uint8_t buff[sizeof(BmpS)];
    }bmpHeadr;
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return 0;
    }
    fread(bmpHeadr.buff, 1, sizeof(bmpHeadr), myImage);
    fclose(myImage);
    return bmpHeadr.file.bmpInfoHeadr.width;
}

uint32_t getImageOfset(uint8_t *imagePath)
{
    union {
        BmpS file;
        uint8_t buff[sizeof(BmpS)];
    }bmpHeadr;
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return 0;
    }
    fread(bmpHeadr.buff, 1, sizeof(bmpHeadr), myImage);
    fclose(myImage);
    return bmpHeadr.file.bmpHeadr.dataOfset;
}

uint32_t fileSize(uint8_t *imagePath)
{
    FILE  *file;
    file = fopen(imagePath, "rb");
    if(!file) {
        printf("Cna't open file\n");
        return 0;
    }
    fseek(file, 0, SEEK_END);
    uint32_t fileSize = ftell(file);
    fclose(file);
    return fileSize;
}

void addTrackToImage(uint8_t *imagePath, pathPoint *rootPath)
{
    FILE *file;
    uint32_t imageFileSize = fileSize(imagePath);
    if(!imageFileSize) {
        printf("Cant Read file\n");
        return;
    }
    uint32_t imageH = getImageHeight(imagePath);
    uint32_t imageW = getImageWidth(imagePath);
    uint32_t imageOfset = getImageOfset(imagePath);
    uint8_t *imageBuff = (uint8_t*)malloc(imageFileSize);
    uint32_t imageWithMem =  (((imageW * sizeof(Color)) % 4) == 0) ? (imageW) : ((((imageW * sizeof(Color)) / 4) + 1) * 4);
    uint8_t (*image)[imageWithMem] = (uint8_t (*)[imageWithMem])(imageBuff + imageOfset);
    if(!(file = fopen(imagePath,"rb")))
    {
        printf("Can't open image file\n");
        return;
    }
    fread(imageBuff, 1, imageFileSize, file);
    fclose(file);
    while(rootPath){
        Color *rawImage = (Color*)image[imageH - rootPath->y];
        rawImage[rootPath->x].r = 0xFF;
        rawImage[rootPath->x].g = 0x0;
        rawImage[rootPath->x].b = 0x0;
        rootPath = rootPath->nexPoint;
    }
    if(!(file = fopen("C:\\image\\test.bmp","w")))
    {
        printf("Can't open test.bmp file\n");
        return;
    }
    fwrite(imageBuff, 1, imageFileSize, file);
    fclose(file);
}

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

int main(int argIn, char **argV)
{
    FILE *imageF;
    const uint8_t *imagePath = myImage;
   // printImageInfo(imagePath);
    uint32_t imageH = getImageHeight(imagePath);
    uint32_t imageW = getImageWidth(imagePath);
    uint32_t imageOfset = getImageOfset(imagePath);
    uint32_t *imageBuff = (uint32_t*)malloc(imageH * imageW * sizeof(uint32_t));
    uint32_t *imageBuffCopy = (uint32_t*)malloc(imageH * imageW * sizeof(uint32_t));
    memset(imageBuff, 0, imageH * imageW * sizeof(uint32_t));
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    uint32_t imageWithMem =  (((imageW * sizeof(Color)) % 4) == 0) ? (imageW) : ((((imageW * sizeof(Color)) / 4) + 1) * 4);
    uint8_t *imageRow = malloc(imageWithMem);

    printf("imageH = %d \n", imageH);
    printf("imageW = %d \n", imageW);
    printf("imageOfset = %d \n", imageOfset);

    imageF = fopen(imagePath, "rb+");
    fseek(imageF, imageOfset, SEEK_SET);
    for(uint32_t k = imageH; k > 0; k--) {
        Color *color = (Color*)imageRow;
        fread(imageRow, 1, imageWithMem, imageF);
        for(uint32_t i = 0; i < imageW; i++) {
            if((color[i].r > 0) || (color[i].g > 0) || (color[i].b > 0)) {
                image[k - 1 ][i] = FREE;
            } else {
                image[k - 1 ][i] = OCCUPIED;
            }
        }
    }
    free(imageRow);
    memcpy((uint8_t*)imageBuffCopy, (uint8_t*)image, imageH * imageW * sizeof(uint32_t));
    //printImage(imageBuffCopy, imageH, imageW);
    optimazeImage(imageBuffCopy,imageH, imageW );



    //printImage(imageBuffCopy, imageH, imageW);
    //return 0;



    uint32_t (*imageBuffCopyP)[imageW] = (uint32_t (*)[imageW])imageBuffCopy;
    initWave(imageBuffCopy, imageH, imageW, startPoint, stopPoint);
    pathPoint *rootPath = findePath(imageBuffCopy, imageH, imageW, startPoint, stopPoint);
    pathPoint *tempPath = rootPath;
    while(tempPath->nexPoint) {
        image[tempPath->y][tempPath->x] = '.';
        tempPath = tempPath->nexPoint;
    }
    addTrackToImage(imagePath, rootPath);
    //printImage(image, imageH, imageW);
    //printWave(imageBuff, imageH, imageW);

    return 0;
}

