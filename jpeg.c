/*
* jpeg.c
*
* Jpeg image file process
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "jpeg.h"

#define DBG(x) (x)

image_t* jpegLoadImage(const char* filename, int iscolor)
{
	int i, ret = 0;
	mat_t *mat = NULL;
	int len = 0;
	FILE *fp;
	unsigned char buf[1024];

	memset(buf, 0, sizeof(buf));

	fp = fopen(filename, "r");
	if (NULL == fp)
	return NULL;

	/* read JPEG HEADER: FFD8*/
	fread(fp, sizeof(char), 2, buf);
	if (buf[0] != 0xFF || buf[1] != 0xD8)
		goto err1;

#define GET_SEG_LEN \	
	fread(fp, sizeof(char), 2, buf); \
	len = buf[0] << 8 | buf[1]; \
	DBG(printf("segment length = %d\n", len))

	while (1) {
		/* continue reading */
		fread(fp, sizeof(char), 2, buf);
		if (buf[0] != 0xFF)
			goto err1;

		switch (buf[1]) {
		case M_SOF0	:
		case M_SOF1	:


		case M_JPG	:
		case M_SOF9	:
		case M_SOF10 :
		case M_SOF11 :

		case M_SOF13 :
		case M_SOF14 :
		case M_SOF15 :

		case M_DHT	:

		case M_DAC	:

		case M_RST0	:
		case M_RST1	:

		//case M_SOI	:
		case M_EOI	:  // end of image
			break;
			
		case M_SOS	:
		case M_DQT	:
		case M_DNL	:
		case M_DRI	:
		case M_DHP	:
		case M_EXP	:

		case M_APP0	:			
			GET_SEG_LEN;
			
			break;
			
		case M_APP1	:

		case M_JPG0	:
		case M_JPG13 :
		case M_COM	:

		case M_TEM	:

		}
	}

    
    return image;  

err1:
	return NULL;
	
}














