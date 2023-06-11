#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <string.h>

#include "labyrinthBmpProcessing.h"
#include "labyrinthProcessing.h"

#define LABYRINTH_DEF_MIN_SEGMENT_SIZE    1
#define LABYRINTH_DEF_DEVIATION           1

typedef enum {
    LABYRINTH_STATUS_OK = 0,
    LABYRINTH_STATUS_CM_ARGYMENT_ERROR = -1,
    LABYRINTH_STATUS_CM_ARGYMENT_START_POINT_EXTENTION_ERROR = -2,
    LABYRINTH_STATUS_CM_ARGYMENT_START_POINT_NUMBER_ERROR = -3,
    LABYRINTH_STATUS_CM_ARGYMENT_STOP_POINT_EXTENTION_ERROR = -4,
    LABYRINTH_STATUS_CM_ARGYMENT_STOP_POINT_NUMBER_ERROR = -5,
    LABYRINTH_STATUS_CM_ARGYMENT_MINIMUM_PATH_SECTION_NUMBER_ERROR = -6,
    LABYRINTH_STATUS_CM_ARGYMENT_MINIMUM_PATH_DEVIATION_NUMBER_ERROR = -7,
    LABYRINTH_STATUS_USER_SETTINGS_NULL_ERROR = -8,
} LabyrinthStatus;

typedef struct {
    char *filePath;
    LabyrinthPixelCoord startPoint;
    LabyrinthPixelCoord stopPoint;
    uint32_t optMinSize;
    uint32_t optDeviation;   
} LabyrinthUserSettings;

uint8_t* createImageWithTrack(const uint8_t *imagePath, LabyrinthPath *path)
{
    FILE *file;
    const uint8_t nameExtension[] = "_with_track.bmp";
    uint32_t imageFileSize = fileSize(imagePath);
    if(!imageFileSize) {
        printf("Cant Read file\n");
        return NULL;
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
        return NULL;
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
        return NULL;
    }
    fwrite(imageBuff, 1, imageFileSize, file);
    fclose(file);
    //free(pathName);
    free(imageBuff);
    return pathName;
}

void createImageWithSmoothTrack(const uint8_t *imagePath, LabyrinthPath *path)
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

    /*
     * The list of points for the optimized path has gaps. We fill gaps by using linear interpolation.
     */
    for(uint32_t k = 1; k < path->length; k++) {
        int32_t x1 = (path->path[k].x > path->path[k - 1].x) ? (path->path[k - 1].x) : (path->path[k].x);
        int32_t y1 = (path->path[k].x > path->path[k - 1].x) ? (path->path[k - 1].y) : (path->path[k].y);
        int32_t x2 = (path->path[k].x > path->path[k - 1].x) ? (path->path[k].x) : (path->path[k - 1].x);
        int32_t y2 = (path->path[k].x > path->path[k - 1].x) ? (path->path[k].y) : (path->path[k - 1].y);;
        if(x2 == x1) {
            continue;
        }
        int32_t y;
        int32_t y_2;
        int32_t k = ((y2 - y1) * 100) / (x2 - x1);
        int32_t b = y2 * 100 - x2 * k;
        int32_t y_1 = (k * x1 + b) / 100;
        for(int32_t x = x1 + 1; x <= x2; x++) {
            y_2 = (k * x + b) / 100;
            for(int32_t y = ((y_1 > y_2) ? (y_2) : (y_1)) ; y <=  ((y_1 > y_2) ? (y_1) : (y_2)); y++) {
                Color *rawImage = (Color*)image[imageH - y - 1];
                rawImage[x].r = 0x0;
                rawImage[x].g = 0x0;
                rawImage[x].b = 0xFF;
            }
            y_1 = y_2;
        }
    }
    printf("track file name: %s\n", imagePath);
    if(!(file = fopen(imagePath,"w")))
    {
        printf("Can't open test.bmp file\n");
        free(imageBuff);
        return;
    }
    fwrite(imageBuff, 1, imageFileSize, file);
    fclose(file);
    free(imageBuff);
}

/*
void printImagePart(uint32_t *imageBuff, uint32_t imageH, uint32_t imageW, uint32_t x, uint32_t y)
{
    printf("PRINT IMAGE \n");
    uint32_t (*image)[imageW] = (uint32_t (*)[imageW])imageBuff;
    printf(" ");
    for(uint32_t k = y; k < imageH; k++) {
        for(uint32_t i = x; i < imageW; i++) {
            if(image[k][i] == OCCUPIED) {
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

volatile LabyrinthPixelColor *imageBuff;
volatile uint32_t imageH;
volatile uint32_t imageW;

bool getPixel(uint32_t x, uint32_t y, LabyrinthPixelColor *labPixel)
{
    if(x >= imageW || y >= imageH) {
        return false;
    }
    LabyrinthPixelColor (*image)[imageW] = (LabyrinthPixelColor (*)[imageW])imageBuff;
    *labPixel = image[y][x];
    return true;
}

int32_t strToUint(uint8_t *str) {
    int32_t rez = 0;
    while(*str) {
        if(*str < '0' && *str > '9' ) {
            return -1;
        }
        rez = rez * 10 + ((*str++) - '0');
    }
    return rez;
}

/*
 * Command line input argument example
 *
 * -f path/to/image/name.bmp -bx 1 -by 1 -ex 50 -ey 50 -s 4 -d 3
 *
 * Description:
 * -f Required. The full path to the target image. Image type: *.bmp 24 bit, full contrast (only black and white collors). Required
 * -bx Required. X coordinate of the start point
 * -by Required. Y coordinate of the start point
 * -ex Required. X coordinate of the stop point
 * -ey Required. Y coordinate of the stop point
 * -s Not required. The minimum segment size of the smoothing. Default value 1
 * -d Not Required. The maximum deviation from the track for the smoothing. Default value 1
 */
int32_t parsCommandLine(int argIn, char **argV, LabyrinthUserSettings *userSettings)
{
    uint32_t k = 0;

    if (userSettings == NULL) {
        return LABYRINTH_STATUS_USER_SETTINGS_NULL_ERROR;
    }

    userSettings->optMinSize = LABYRINTH_DEF_MIN_SEGMENT_SIZE;
    userSettings->optDeviation = LABYRINTH_DEF_DEVIATION;

    while(++k < argIn) {
        if(argV[k][0] != '-') {
            return LABYRINTH_STATUS_CM_ARGYMENT_ERROR;
        }

        switch(argV[k][1]) {
        case 'f': // path to file
            k++;
            userSettings->filePath = malloc(strlen(argV[k]) + 1);
            strcpy(userSettings->filePath, argV[k]);
            break;

        case 'b': { // start point
            uint32_t *coord;

            switch(argV[k][2]) {
            case 'x':
                coord = &userSettings->startPoint.x;
                break;
            case 'y':
                coord = &userSettings->startPoint.y;
                break;
            default:
                return LABYRINTH_STATUS_CM_ARGYMENT_START_POINT_EXTENTION_ERROR;
            }
            k++;

            if((*coord = strToUint(argV[k])) < 0) {
                return LABYRINTH_STATUS_CM_ARGYMENT_START_POINT_NUMBER_ERROR;
            }
            break;
        }

        case 'e': { // stop point
            uint32_t *coord;

            switch(argV[k][2]) {
            case 'x':
                coord = &userSettings->stopPoint.x;
                break;

            case 'y':
                coord = &userSettings->stopPoint.y;
                break;

            default:
                return LABYRINTH_STATUS_CM_ARGYMENT_STOP_POINT_EXTENTION_ERROR;
            }
            k++;

            if((*coord = strToUint(argV[k])) < 0) {
                return LABYRINTH_STATUS_CM_ARGYMENT_STOP_POINT_NUMBER_ERROR;
            }
            break;
            }

        case 's': // minimus size of optimization path
            k++;
            if((userSettings->optMinSize = strToUint(argV[k])) < 0) {
                return LABYRINTH_STATUS_CM_ARGYMENT_MINIMUM_PATH_SECTION_NUMBER_ERROR;
            }
            break;

        case 'd': // optimization deviation
            k++;
            if((userSettings->optDeviation = strToUint(argV[k])) < 0) {
                return LABYRINTH_STATUS_CM_ARGYMENT_MINIMUM_PATH_DEVIATION_NUMBER_ERROR;
            }
            break;
        }
    }

    return LABYRINTH_STATUS_OK;
}

int main(int argIn, char **argV)
{
    FILE *imageF;
    uint32_t k = 0;
    LabyrinthStatus rez;
    LabyrinthUserSettings userSettings;

    rez = parsCommandLine(argIn, argV, &userSettings);

    if (rez != LABYRINTH_STATUS_OK) {
        return -1;
    }

    if(!isFileExist(userSettings.filePath)) {
        return 0;
    }
    imageH = getImageHeight(userSettings.filePath);
    imageW = getImageWidth(userSettings.filePath);
    uint32_t imageOfset = getImageOfset(userSettings.filePath);
    imageBuff = (LabyrinthPixelColor*)malloc(imageH * imageW * sizeof(LabyrinthPixelColor));
    LabyrinthPixelColor (*image)[imageW] = (LabyrinthPixelColor (*)[imageW])imageBuff;
    uint32_t imageWithMem =  (((imageW * sizeof(Color)) % 4) == 0) ?
                               (imageW * sizeof(Color)) :
                               ((((imageW * sizeof(Color)) / 4) + 1) * 4);
    uint8_t *imageRow = malloc(imageWithMem);

    /*
     * Read image
     */
    imageF = fopen(userSettings.filePath, "rb+");
    fseek(imageF, imageOfset, SEEK_SET);
    for(uint32_t k = imageH; k > 0; k--) {
        LabyrinthPixelColor *color = (LabyrinthPixelColor*)imageRow;
        fread(imageRow, 1, imageWithMem, imageF);
        for(uint32_t i = 0; i < imageW; i++) {
             image[k - 1][i] = color[i];
        }
    }
    free(imageRow);

    /*
     * Labirynt processing
     */
    LabyrinthLabP lab = labyrinthInit(getPixel);
    if(lab == NULL) {
        printf("Cant create labP \n");
        return -1;
    }

    if(!labyrinthReadImage(lab)) {
        printf("Cant read image \n");
        return -1;
    }

    LabyrinthPath* path = labyrinthGetPath(lab, userSettings.startPoint, userSettings.stopPoint);
    if(path == NULL) {
        printf("Can't finde path \n");
        return 0;
    }
    printf("Not smooth path size = %u\n", path->length);
    uint8_t *imageWithTrack = createImageWithTrack(userSettings.filePath, path);


    LabyrinthPath* smoothPath = labyrinthSmoothPath(path,
                                                    userSettings.optMinSize,
                                                    userSettings.optDeviation);
    if(smoothPath == NULL) {
        printf("Can't smooth path\n");
        return 0;
    }
    printf("Optimaze path size = %u\n", smoothPath->length);
    createImageWithSmoothTrack(imageWithTrack, smoothPath);

    return 0;
}

