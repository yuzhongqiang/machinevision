/*
* types.h
*
* Base data types
*/

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char u8;
typedef unsigned unsigned short u16;
typedef unsigned int u32;

/****************************************************************************************\
*                                  Matrix type (mat_t)                                   *
\****************************************************************************************/

/*
  mat_t type definition
  bit0 ~ bit1: channels
  bit8 ~ bit10: element type
*/

#define MV_CHN_MAX    4
#define MV_CHN_BITS   3
#define MV_CHN_SHIFT  0
#define MV_CHN_MASK  ((1<<MV_CHN_BITS) - 1)
#define MV_CHN(chn) ((chn) << MV_CHN_SHIFT)

#define MV_8U   0
#define MV_8S   1
#define MV_16U  2
#define MV_16S  3
#define MV_32S  4
#define MV_32F  5
#define MV_64F  6
#define MV_USRTYPE1 7
#define MV_USRTYPE2 8

#define MV_DEPTH_SHIFT 8
#define MV_DEPTH_BITS   6
#define MV_DEPTH_MASK  (((1<<MV_DEPTH_BITS) - 1) << MV_DEPTH_SHIFT)
#define MV_DEPTH(depth)  ((depth) << MV_DEPTH_SHIFT)

#define MV_MAKETYPE(depth,chn) (MV_DEPTH(depth) + MV_CHN(chn))

#define MV_8UC1 MV_MAKETYPE(MV_8U,1)
#define MV_8UC2 MV_MAKETYPE(MV_8U,2)
#define MV_8UC3 MV_MAKETYPE(MV_8U,3)
#define MV_8UC4 MV_MAKETYPE(MV_8U,4)

#define MV_8SC1 MV_MAKETYPE(MV_8S,1)
#define MV_8SC2 MV_MAKETYPE(MV_8S,2)
#define MV_8SC3 MV_MAKETYPE(MV_8S,3)
#define MV_8SC4 MV_MAKETYPE(MV_8S,4)

#define MV_16UC1 MV_MAKETYPE(MV_16U,1)
#define MV_16UC2 MV_MAKETYPE(MV_16U,2)
#define MV_16UC3 MV_MAKETYPE(MV_16U,3)
#define MV_16UC4 MV_MAKETYPE(MV_16U,4)

#define MV_16SC1 MV_MAKETYPE(MV_16S,1)
#define MV_16SC2 MV_MAKETYPE(MV_16S,2)
#define MV_16SC3 MV_MAKETYPE(MV_16S,3)
#define MV_16SC4 MV_MAKETYPE(MV_16S,4)

#define MV_32SC1 MV_MAKETYPE(MV_32S,1)
#define MV_32SC2 MV_MAKETYPE(MV_32S,2)
#define MV_32SC3 MV_MAKETYPE(MV_32S,3)
#define MV_32SC4 MV_MAKETYPE(MV_32S,4)

#define MV_32FC1 MV_MAKETYPE(MV_32F,1)
#define MV_32FC2 MV_MAKETYPE(MV_32F,2)
#define MV_32FC3 MV_MAKETYPE(MV_32F,3)
#define MV_32FC4 MV_MAKETYPE(MV_32F,4)

#define MV_64FC1 MV_MAKETYPE(MV_64F,1)
#define MV_64FC2 MV_MAKETYPE(MV_64F,2)
#define MV_64FC3 MV_MAKETYPE(MV_64F,3)
#define MV_64FC4 MV_MAKETYPE(MV_64F,4)

typedef struct mat_t
{
    u32 type;
    int step;

    /* for internal use only */
    int* refcount;
    int hdr_refcount;

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
}mat_t;

inline mat( int rows, int cols, int type, void* data)
{
    CvMat m;

    assert( (unsigned)MV_MAT_DEPTH(type) <= MV_64F );
    type = MV_MAT_TYPE(type);
    m.type = MV_MAT_MAGIC_VAL | MV_MAT_CONT_FLAG | type;
    m.cols = cols;
    m.rows = rows;
    m.step = m.cols*MV_ELEM_SIZE(type);
    m.data.ptr = (uchar*)data;
    m.refcount = NULL;
    m.hdr_refcount = 0;

    return m;
}


#define MV_MAT_ELEM_PTR_FAST( mat, row, col, pix_size )  \
    (assert( (unsigned)(row) < (unsigned)(mat).rows &&   \
             (unsigned)(col) < (unsigned)(mat).cols ),   \
     (mat).data.ptr + (size_t)(mat).step*(row) + (pix_size)*(col))

#define MV_MAT_ELEM_PTR( mat, row, col )                 \
    MV_MAT_ELEM_PTR_FAST( mat, row, col, MV_ELEM_SIZE((mat).type) )

#define MV_MAT_ELEM( mat, elemtype, row, col )           \
    (*(elemtype*)MV_MAT_ELEM_PTR_FAST( mat, row, col, sizeof(elemtype)))


MV_INLINE  double  cvmGet( const CvMat* mat, int row, int col )
{
    int type;

    type = MV_MAT_TYPE(mat->type);
    assert( (unsigned)row < (unsigned)mat->rows &&
            (unsigned)col < (unsigned)mat->cols );

    if( type == MV_32FC1 )
        return ((float*)(mat->data.ptr + (size_t)mat->step*row))[col];
    else
    {
        assert( type == MV_64FC1 );
        return ((double*)(mat->data.ptr + (size_t)mat->step*row))[col];
    }
}


MV_INLINE  void  cvmSet( CvMat* mat, int row, int col, double value )
{
    int type;
    type = MV_MAT_TYPE(mat->type);
    assert( (unsigned)row < (unsigned)mat->rows &&
            (unsigned)col < (unsigned)mat->cols );

    if( type == MV_32FC1 )
        ((float*)(mat->data.ptr + (size_t)mat->step*row))[col] = (float)value;
    else
    {
        assert( type == MV_64FC1 );
        ((double*)(mat->data.ptr + (size_t)mat->step*row))[col] = (double)value;
    }
}


MV_INLINE int cvIplDepth( int type )
{
    int depth = MV_MAT_DEPTH(type);
    return MV_ELEM_SIZE1(depth)*8 | (depth == MV_8S || depth == MV_16S ||
           depth == MV_32S ? IPL_DEPTH_SIGN : 0);
}

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
    char *data;        /* Pointer to aligned image data.         */
} image_t;









#endif  // _TYPES_H_

