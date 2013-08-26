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


seg_app0_t *app0;// = {{0xFF, 0xE0}, };
seg_com_t  *com;// = {{0xFF, 0xFE},};
seg_dqt_t *dqt;// = {{0xFF, 0xDB},};
seg_sof0_t *sof0;// = {{0xFF, 0xC0},};
seg_dri_t *dri;// = {{0xFF, 0xDD},};
seg_sos_t *sos;// = {{0xFF, 0xDA},};
seg_dht_t *dht;// = {{0xFF, 0xC4},};



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

	/* read SOI: FFD8*/
	fread(fp, sizeof(char), 2, buf);
	if (buf[0] != 0xFF || buf[1] != 0xD8)
		goto err1;

#define GET_SEG_LEN \	
	fread(fp, sizeof(char), 2, buf); \
	len = buf[0] << 8 | buf[1]; \
	DBG(printf("segment length = %d\n", len))

	while (1) {
		/* read segment header */
		fread(fp, sizeof(char), 2, buf);
		if (buf[0] != 0xFF)
			goto err1;

		switch (buf[1]) {
			case M_SOF0	:

				
				break;

			case M_JPG	:
			case M_SOF9	:
			case M_SOF10 :
			case M_SOF11 :



			case M_DHT	:


			case M_SOI	:
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
				app0 = (seg_app0_t*)malloc(len+2);
				app0->hdr.flag = 0xFF;
				app0->hdr.id = 0xE0;
				fread(fp, sizeof(char), len-2, app0->jfif);				
				break;
				
			case M_COM	:
				com = (seg_com_t*)malloc(len+2);
				com->hdr.flag = 0xFF;
				com->hdr.id = 0xFE;
				fread(fp, sizeof(char), len-2, com->jfif);	
				break;

			default:
				break;
		}
	}

    
    return image;  

err1:
	return NULL;
	
}














