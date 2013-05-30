/*
* Image.h
* Image process function property
*/

#ifndef _IMAGE_H_
#define _IMAGE_H_


typedef struct image_codec_s {

} image_codec_t;

image_t* mvLoadImage(const char* filename, int iscolor);
//int mvSaveImage(const char* filename, image_t *image, int iscolor);



#endif //_IMAGE_H_

