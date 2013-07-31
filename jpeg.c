/*
* jpeg.c
*
* Jpeg image file process
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "jpeg.h"


image_t* jpegLoadImage(const char* filename, int iscolor)
{
	int i, ret = 0;
	mat_t *mat = NULL;
	FILE *fp;
	unsigned char buf[1024];

	memset(buf, 0, sizeof(buf));

	fp = fopen(filename, "r");
	if (NULL == fp)
	return NULL;

	/* read HEAD: FFD8*/
	fread(fp, sizeof(char), 2, buf);

	/* continue reading */
	fread(fp, sizeof(char), 2, buf);

	switch (buf[1]) {
case M_SOF0	:
case M_SOF1	:
case M_SOF2	:
case M_SOF3	:

case M_SOF5	:
case M_SOF6	:
case M_SOF7	:

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
case M_RST2	:
case M_RST3	:
case M_RST4	:
case M_RST5	:
case M_RST6	:
case M_RST7	:

case M_SOI	:
case M_EOI	:
case M_SOS	:
case M_DQT	:
case M_DNL	:
case M_DRI	:
case M_DHP	:
case M_EXP	:

case M_APP0	:
case M_APP1	:
case M_APP2	:
case M_APP3	:
case M_APP4	:
case M_APP5	:
case M_APP6	:
case M_APP7	:
case M_APP8	:
case M_APP9	:
case M_APP10 :
case M_APP11 :
case M_APP12 :
case M_APP13 :
case M_APP14 :
case M_APP15 :

case M_JPG0	:
case M_JPG13 :
case M_COM	:

case M_TEM	:

		}


    
    return image;    
}














