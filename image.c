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

image_t* mvCreateImageHeader(int width, int height, int channels)
{
    return NULL;
}

image_t* mvCreateImage(int width, int height, int channels)
{
    return NULL;
}

/* Only create image header */
image_t* mvCreateImageHeader(int width, int height, int channels, int depth)
{
	image_t *image = NULL;
	int ret;

	image = malloc(sizeof(image_t));
	if (NULL == image) {
		return E_NOMEM;
	}

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->depth = depth;

	return image;
}

void mvReleaseImageHeader(image_t *image)
{
	free(image);
}

/* Create image structure and allocate data space */
image_t* mvCreateImage(int width, int height, int channels, int depth)
{
	image_t *image = NULL;
	int ret;

	image = malloc(sizeof(image_t));
	if (NULL == image) {
		return E_NOMEM;
	}

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->depth = depth;

	return image;
}



IMAGE_TYPE mvGetImageType(const char* filename)
{
	int i, j = strlen(filename) - 1;
	IMAGE_TYPE type;

	if (NULL == filename || 0 == j)
		return E_PARAM;
	
	while (*(filename+j) == 0x20 && j > 0)
		j--;

    /* all-blankspace file name */
    if (j == 0)
        return E_PARAM;        
    
    i = j;
    while (*(filename+i) != '.' && i > 0)
        i--;

    /* file has no extend name */
    if ( 0 == i)
        return TYPE_UNKNOWN;

    if (!strncmp(filename+i+1, "bmp", j-i))
        return TYPE_BMP;
    else if (!strncmp(filename+i+1, "jpg", j-i) || !strncmp(filename+i+1, "jpeg", j-i))
        return TYPE_JPEG;
    else
        return TYPE_UNKNOWN;
}


image_t* mvLoadImage(const char* filename, int iscolor)
{
	image_t* img = NULL;
    IMAGE_TYPE type;

    type = mvGetImageType(filename);
    if (type < 0)
        return E_PARAM;

    switch (type) {
    case TYPE_BMP:
        img = bmpLoadImage(filename, iscolor);
        break;

    case TYPE_JPEG:
        //img = jpegLoadImage(filename, iscolor);
        break;

    case TYPE_UNKNOWN:
        break;
    }

    return img;
}

int mvSaveImage(const char* filename, image_t* image, int iscolor)
{



}













