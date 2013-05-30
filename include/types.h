/*
* types.h
*
* Base data types
*/

#ifndef _TYPES_H_
#define _TYPES_H_



typedef unsigned char uchar;


/* Matrix defination */

typedef struct mat_s {
    int type;
    int step;

    union
    {
        uchar* ptr;
        short* s;
        int* i;
        float* fl;
        double* db;
    } data;

    int rows;
    int cols;
} mat_t;



/* Point defination */

typedef struct point2i_s {
	int x;
	int y;
} point2i_t;

typedef struct point2f_s {
	float x;
	float y;
} point2df_t;

typedef struct point2d_s {
	double x;
	double y;
};


typedef struct point3_s {
	int x;
	int y;
	int z;
} point2d_t;

typedef struct point3f_s {
	float x;
	float y;
	float z;
} point2df_t;

typedef struct point3d_s {
	double x;
	double y;
	double z;
} point3f_t;


/* Image structure definition */

typedef struct image_s
{
    int  channels;         /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int  depth;             /* Pixel depth in bits:  */
    int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
    int  imageSize;         /* Image data size in bytes */
    char *imageData;        /* Pointer to aligned image data.         */
} image_t;











#endif  // _TYPES_H_

