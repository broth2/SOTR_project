#ifndef IPA
#define IPA
#include <stdint.h>
#define IMGWIDTH 16

int guideLineSearch(uint8_t imageBuf[IMGWIDTH][IMGWIDTH], int16_t *pos, float *angle);
int obstCount(uint8_t imageBuf[IMGWIDTH][IMGWIDTH]);
int csaObjects(uint8_t imageBuf[128][128]);
void test();

#endif