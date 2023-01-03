#ifndef IPA
#define IPA
#include <stdint.h>
#define IMGWIDTH 128

int guideLineSearch(uint8_t imageBuf[IMGWIDTH][IMGWIDTH], int16_t *pos, float *angle);
int obstCount(uint8_t imageBuf[IMGWIDTH][IMGWIDTH]);
int csaObjects(uint8_t imageBuf[IMGWIDTH][IMGWIDTH]);
void test();

#endif