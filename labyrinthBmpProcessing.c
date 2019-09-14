#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "labyrinthBmpProcessing.h"

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

uint32_t getImageSize(const uint8_t *imagePath)
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

uint32_t getImageHeight(const uint8_t *imagePath)
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

uint32_t getImageWidth(const uint8_t *imagePath)
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

uint32_t getImageOfset(const uint8_t *imagePath)
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

uint32_t fileSize(const uint8_t *imagePath)
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

bool isFileExist(const uint8_t *imagePath)
{
    FILE *myImage;
    myImage = fopen(imagePath, "rb");
    if(!myImage) {
        printf("ERROR, can't open file");
        return false;
    }
    fclose(myImage);
    return true;
}
