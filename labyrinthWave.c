#include <stdint.h>
#include <stdbool.h>

#include "labyrinthWave.h"
#define WAVE_SIZE 10000


struct WaveS {
    uint32_t cnt;
    struct {
        uint32_t x;
        uint32_t y;
    } pos[WAVE_SIZE];
};

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
