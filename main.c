#include <stdio.h>
#include <stdint.h>

#include "bmpDescriptions.h"
#include "labyrinthWave.h"

const uint8_t  myImage[] = "C:\\image\\lab_1.bmp";


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

void imageAddWave(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, uint32_t *waveBuff)
{

}

void printImage(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW)
{
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    for(uint32_t k = 0; k < imageH; k++) {
        for(uint32_t i = 0; i < imageW; i++) {
            if(image[k][i] == 0xFFFFFFFF) {
                printf("%s", "#");
            } else if (image[k][i] == '.') {
                printf("%s", ".");
            } else {
                printf("%s", " ");
            }
            //printf("%s", (image[k][i]) ? ("#") : (" "));
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
    for(uint8_t k = imageH; k > 0; k--) {
        Color *color = (Color*)imageRow;
        fread(imageRow, 1, imageWithMem, imageF);
        for(uint32_t i = 0; i < imageW; i++) {
            if((color[i].r > 0) || (color[i].g < 0) || (color[i].b > 0)) {
                image[k - 1 ][i] = FREE;
            } else {
                image[k - 1 ][i] = OCCUPIED;
            }
        }
    }
    free(imageRow);
    memcpy((uint8_t*)imageBuffCopy, (uint8_t*)imageBuff, imageH * imageW * sizeof(uint32_t));
    uint32_t (*imageBuffCopyP)[imageW] = (uint32_t (*)[imageW])imageBuffCopy;
    printImage(imageBuffCopy, imageH, imageW);
    initWave(imageBuff, imageH, imageW, (point2D){4, 5}, (point2D){46, 34});
    pathPoint *rootPath = findePath(imageBuff, imageH, imageW, (point2D){4, 5}, (point2D){46, 34});
    pathPoint *tempPath = rootPath;
    while(tempPath->nexPoint) {
        //printf("x = %4u, y = %4u \n", tempPath->x, tempPath->y);
        imageBuffCopyP[tempPath->y][tempPath->x] = '.';
        tempPath = tempPath->nexPoint;
    }
    printImage(imageBuffCopy, imageH, imageW);
    //printWave(imageBuff, imageH, imageW);

    return 0;
}
