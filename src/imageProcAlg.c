/* The usual includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "ipa.h"

/* Some defines that are usefull to make the SW more readable */
/* and adaptable */ 
#define IMGWIDTH 16 /* Square image. Side size, in pixels*/
#define BACKGROUND_COLOR 0x00 /* Color of the background */
#define GUIDELINE_COLOR 0xFF  /* Guideline color */
#define OBSTACLE_COLOR 0x80 /* Obstacle color */
#define GN_ROW 0	/* Row to look for the guiode line - close */
#define GF_ROW (IMGWIDTH-1)	/* Row to look for the guiode line - far */
#define NOB_ROW 3	/* Row to look for near obstacles */
#define NOB_COL 5	/* Col to look for near obstacles */
#define NOB_WIDTH 5	/* WIDTH of the sensor area */
#define PI 3.141593 /* PI with  6 decimal cases */
#define CSA_TOP (IMGWIDTH/2)	/* CSA top outside border  */
#define CSA_LEFT (IMGWIDTH/4)	/* CSA left outside border  */
#define CSA_RIGHT (3*IMGWIDTH/4)	/* CSA right outside border  */

/* One example image. In raw/gray format an image is an array of 
 * bytes, one per pixel, with values that represent intensity and range
 * from black (0x00) to bright white (0xFF). 
 * In the example image below you can see the image
 * pixels/bytes organized in rows-columns (matrix format). It is not 
 * necessary for the SW. I put it this way just to facilitate 
 * this explanation.
 * You can see on the 8th column a white stripe (column of pixels with 
 * 0xFF). On the bottom-righ, two last rows, you can see two pixels 
 * of gray color, that represent two obstacles. 
 */
uint8_t img1[IMGWIDTH][IMGWIDTH]= 
	{	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0xFF, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00},					
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00} 
	};


/* Function that detects he position and agle of the guideline */
int guideLineSearch(uint8_t imageBuf[IMGWIDTH][IMGWIDTH], int16_t *pos, float *angle) {
	int i, gf_pos;
	float divergence, pos_percent;
		
	/* Inits */
	pos_percent=-1;
	gf_pos=-1;
		
	/* Search for guideline pos - Near*/
	for(i=0; i < IMGWIDTH; i++) {
		if(imageBuf[GN_ROW][i] == GUIDELINE_COLOR) {
			pos_percent = i;
			break;
		}			
	}
	
	/* Search for guideline pos - Far*/
	for(i=0; i < IMGWIDTH; i++) {
		if(imageBuf[GF_ROW][i] == GUIDELINE_COLOR) {
			gf_pos = i;
			break;
		}			
	}

	if(pos_percent == -1 || gf_pos == -1) {
		printf("Failed to find guideline pos=%d, gf_pos=%d", (int)pos_percent, gf_pos);
		return -1;
	}

	// angle calculation, assumes img is square
	divergence = (pos_percent-gf_pos)/(IMGWIDTH-1);
	*angle = divergence * (PI/4); 
	
	// position percentage
	*pos = (pos_percent/(IMGWIDTH-1)) * 100;

	return 0;	
}

/* Function that counts obstacles. 
   Crude version. Only works if one obstacle per row at max. */
int obstCount(uint8_t imageBuf[IMGWIDTH][IMGWIDTH]) {
	int i, j, nobs;
		
	/* Inits */
	nobs=0;
		
	/* Search for obstacles. Crude version. Only works if one obstacle per row at max*/
	for(j=0; j < IMGWIDTH; j++) {
		for(i=0; i < IMGWIDTH; i++) {
			if(imageBuf[j][i] == OBSTACLE_COLOR) {
				nobs++;
			}			
		}
	}
	
	return nobs;
}

/* Function to count detected obstacles in CSA*/
int csaObjects(uint8_t imageBuf[128][128]){
	for(int i=0; i<CSA_TOP; i++){
		for(int j=CSA_LEFT; j<CSA_RIGHT; j++){
			if(imageBuf[i][j] == OBSTACLE_COLOR) {
				return 1;
			}
		}
	}

	return 0;
}

void test(){
	int16_t pos;
	float angle, static_angle =(((float)(8-7)/(16-1)) * (PI/4));

	guideLineSearch(img1, &pos, &angle);
	assert(5==obstCount(img1));
	assert(1==csaObjects(img1));

	assert(46==pos);
	assert(static_angle==angle);
	static_angle++;

	printf("Passed tests!\n\r");
}

/* Main function */
// int main() {	
	
	// printf("Test for image processing algorithms \n\r");
	// test();
	// printf("Detecting position and guideline angle ...\n\r");
	// res = guideLineSearch(img1, &pos, &angle);
	// printf("Robot position=%d, guideline angle = %f\n\r",pos,angle);
	
	// printf("Detecting number of obstacles ...");
	// res = obstCount(img1);
	// printf("%d obstacles detected\n\r",res);

	// res = csaObjects(img1, &csa_obj);
	// printf("%d obsctacles in CSA\n\r", csa_obj);
	
// }
