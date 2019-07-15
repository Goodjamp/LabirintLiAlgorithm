/*
 * Author: Gerasimchyk
 * File:   bmpDescriptions.h
 * Date:   15.07.2019
*/

#ifndef _BMP_DESCRIPTIONS_
#define _BMP_DESCRIPTIONS_

#include <stdint.h>

#pragma pack(push, 1)
typedef struct BmpS{
    struct {
        uint16_t signature;
        uint32_t fileSize;
        uint32_t reserved;
        uint32_t dataOfset;
    } bmpHeadr;
    struct {
        uint32_t size;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bitsPerixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t xPixelPerM;
        uint32_t yPixelPerM;
        uint32_t colorUsed;
        uint32_t ImportantColors;
    } bmpInfoHeadr;
}BmpS;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
    } Color;
#pragma pack(pop)

#endif
