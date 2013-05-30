/*
* image.c
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "types.h"
#include "image.h"

extern image_codec_t image_codec;

image_t* mvLoadImage(const char* filename, int iscolor)
{
	int ret;

	return image_codec.LoadImage(filename, iscolor);	
}

/*
int mvSaveImage(const char* filename, image_t* image, int iscolor)
{

}
*/
http://www.letv.com/ptv/vplay/1607365.html?q2=baofeng


