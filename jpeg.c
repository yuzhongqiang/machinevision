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
	image_t * image;

	memset(buf, 0, sizeof(buf));

	fp = fopen(filename, "r");
	if (NULL == fp)
	    return NULL;

	/* read SOI: FFD8*/
	fread(buf, sizeof(char), 2, fp);
	if (buf[0] != 0xFF || buf[1] != 0xD8)
		goto err1;

#define GET_SEG_LEN \	
	fread(buf, sizeof(char), 2, fp); \
	len = buf[0] << 8 | buf[1]; \
	DBG(printf("segment length = %d\n", len))

	while (1) {
		/* read segment header */
		fread(buf, sizeof(char), 2, fp);
		if (buf[0] != 0xFF)
			goto err1;

		switch (buf[1]) {
			case M_SOF0	:
				GET_SEG_LEN;
				sof0 = (seg_sof0_t*)malloc(len+2);
				sof0->hdr.flag = 0xFF;
				sof0->hdr.id = 0xC0;
                sof0->length = len;
				fread(&sof0->pricise, sizeof(char), len-2, fp);
                sof0->height = ntohs(sof0->height);
                sof0->width = ntohs(sof0->width);
				break;

			case M_DHT	:
				GET_SEG_LEN;
				dht = (seg_dht_t*)malloc(len+2);
				dht->hdr.flag = 0xFF;
				dht->hdr.id = 0xDB;
                dht->length = len;
				fread(&dht->info, sizeof(char), len-2, fp); 
				break;

			case M_SOI	:break;
			case M_EOI	:break;  // end of image
				break;
				
			case M_SOS	:
				GET_SEG_LEN;
				sos = (seg_sos_t*)malloc(len+2);
				sos->hdr.flag = 0xFF;
				sos->hdr.id = 0xDA;
                sos->length = len;
				fread(&sos->comp_count, sizeof(char), len-2, fp);	
				break;
				
			case M_DQT	:
				GET_SEG_LEN;
				dqt = (seg_dqt_t*)malloc(len+2);
				dqt->hdr.flag = 0xFF;
				dqt->hdr.id = 0xDB;
                dqt->length = len;
				fread(&dqt->info, sizeof(char), len-2, fp);				
				break;
				
			case M_DRI	:
				GET_SEG_LEN;
				dri = (seg_dqt_t*)malloc(len+2);
				dri->hdr.flag = 0xFF;
				dri->hdr.id = 0xDD;
                dri->length = len;
				fread(&dri->interval, sizeof(char), len-2, fp);	
                dri->interval = ntohs(dri->interval);
				break;
				
			case M_APP0	:			
				GET_SEG_LEN;
				app0 = (seg_app0_t*)malloc(len+2);
				app0->hdr.flag = 0xFF;
				app0->hdr.id = 0xE0;
                app0->length = len;
				fread(app0->jfif, sizeof(char), len-2, fp);				
                app0->xdensity = ntohs(app0->xdensity);
                app0->ydensity = ntohs(app0->ydensity);
				break;
				
			case M_COM	:
				com = (seg_com_t*)malloc(len+2);
				com->hdr.flag = 0xFF;
				com->hdr.id = 0xFE;
                com->length = len;
				fread(com->comment, sizeof(char), len-2, fp);	
				break;

			default:
				break;
		}
	}

    fclose(fp);
    return image;  

err1:
	return NULL;
	
}














