/*
* Image.h
* Image process function property
*/

#ifndef _IMAGE_H_
#define _IMAGE_H_

typedef enum {TYPE_UNKNOWN=0, TYPE_BMP=1, TYPE_JPEG, TYPE_MAX} IMAGE_TYPE;

image_t* mvLoadImage(const char* filename, int iscolor);
int mvSaveImage(const char* filename, image_t *image, int iscolor);

IMAGE_TYPE mvGetImageType(const char* filename);


#endif //_IMAGE_H_

