/*
* types.h
*
* Base data types
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#define MV_INLINE inline
#define MV_STATIC static

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long int64;

/**********************************************************************************\
*                                                     Error code definition                                                        *
\**********************************************************************************/
typedef enum err_code {
    ERR_PARAM,
    ERR_NOMEM,
    ERR_NOFILE,
    ERR_NULLPTR,
}err_code;


/****************************************************************************************\
*                                  Matrix type (mat_t)                                   *
\****************************************************************************************/

/*
  mat_t type definition
  bit0 ~ bit1: channels
  bit8 ~ bit10: element type
*/

#define MAT_CHN_MAX    4
#define MAT_CHN_BITS   3
#define MAT_CHN_SHIFT  0
#define MAT_CHN_MASK  ((1<<MAT_CHN_BITS) - 1)
#define MAT_MAKECHN(chn) ((chn) << MAT_CHN_SHIFT)
#define MAT_ELEM_CHN(type) ((type) & (MAT_CHN_MASK))

/* element single channel depth */
#define MAT_8U     0
#define MAT_8S     1
#define MAT_16U   2
#define MAT_16S   3
#define MAT_32U   4
#define MAT_32S   5

#define MAT_DEPTH_SHIFT 8
#define MAT_DEPTH_BITS   6
#define MAT_DEPTH_MASK  (((1<<MAT_DEPTH_BITS) - 1) << MAT_DEPTH_SHIFT)
#define MAT_MAKEDEPTH(depth)  ((depth) << MAT_DEPTH_SHIFT)
#define MAT_ELEM_DEPTH(type) (((type) & MAT_DEPTH_MASK) >> MAT_DEPTH_SHIFT)

#define MAT_MAKETYPE(depth,chn) (MAT_MAKEDEPTH(depth) + MAT_MAKECHN(chn))

#define MAT_8UC1 MAT_MAKETYPE(MAT_8U,1)
#define MAT_8UC2 MAT_MAKETYPE(MAT_8U,2)
#define MAT_8UC3 MAT_MAKETYPE(MAT_8U,3)
#define MAT_8UC4 MAT_MAKETYPE(MAT_8U,4)

#define MAT_ELEM_TYPE(x)

#define MAT_ELEM_SIZE(type) ((MAT_ELEM_DEPTH(type))/2+1)*MAT_ELEM_CHN(type)

typedef struct mat_t
{
    u32 type;
    int step;   /* matrix row step in bytes */

    union
    {
        u8* ptr;
        short* s;
        int* i;
        float* fl;
        double* db;
    } data;

    int rows;
    int cols;
}mat_t;

typedef struct point2i_t {
	int x;
	int y;
} point2i_t;

typedef struct point2f_t {
	float x;
	float y;
} point2f_t;

typedef struct point2d_t {
	double x;
	double y;
}point2d_t;


typedef struct point3i_t {
	int x;
	int y;
	int z;
} point3i_t;

typedef struct point3f_t {
	float x;
	float y;
	float z;
} point3f_t;

typedef struct point3d_t {
	double x;
	double y;
	double z;
} point3d_t;

/**********************************************************************************\
*                                                 Image structure definition                                                    *
\**********************************************************************************/

#define IMG_DEPTH_SIGN 0x80000000

#define IMG_DEPTH_1U     1
#define IMG_DEPTH_8U     8
#define IMG_DEPTH_16U   16

typedef struct image_t {
    int  size;        /* size of struct image_t */
    int  chn;         /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int  depth;     /* Bit count of per channel per pixel */
    
    int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
    int  img_size;         /* Image data size in bytes */
    char *data;        /* Pointer to aligned image data. */
} image_t;



#endif  // _TYPES_H_

