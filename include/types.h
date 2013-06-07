/*
* types.h
*
* Base data types
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#define MV_INLINE inline
#define int64 long long;

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
#define MV_MAKECHN(chn) ((chn) << MV_CHN_SHIFT)
#define MV_ELEM_CHN(type) ((type) & (MV_CHN_MASK))

#define MV_8U   0
#define MV_8S   1
#define MV_16U  2
#define MV_16S  3
#define MV_32S  4
#define MV_32F  5
#define MV_64F  6
#define MV_DEPTH_MAX  MV_64F

#define MV_DEPTH_SHIFT 8
#define MV_DEPTH_BITS   6
#define MV_DEPTH_MASK  (((1<<MV_DEPTH_BITS) - 1) << MV_DEPTH_SHIFT)
#define MV_MAKEDEPTH(depth)  ((depth) << MV_DEPTH_SHIFT)
#define MV_ELEM_DEPTH(type) (((type) & MV_DEPTH_MASK) >> MV_DEPTH_SHIFT)

#define MV_MAKETYPE(depth,chn) (MV_MAKEDEPTH(depth) + MV_MAKECHN(chn))

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

#define MV_ELEM_SIZE(type) (((MV_ELEM_DEPTH(type))/2+1)  \
            * ((MV_ELEM_CHN(type)) == 3 ? 4 : (MV_ELEM_CHN(type))))

typedef struct mat_t
{
    u32 type;
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
}mat_t;

MV_INLINE mat( int rows, int cols, int type, void* data)
{
    CvMat m;

    assert( (u32)MV_ELEM_DEPTH(type) <= MV_64F );
    m.cols = cols;
    m.rows = rows;
    m.step = m.cols*MV_ELEM_SIZE(type);
    m.data.ptr = (u8*)data;

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








/***********************************************************************\
*                                                  Error code                                                       *
\***********************************************************************/
enum {
 MV_StsOk=                       0,  /* everithing is ok                */
 MV_StsBackTrace=               -1,  /* pseudo error for back trace     */
 MV_StsError=                   -2,  /* unknown /unspecified error      */
 MV_StsInternal=                -3,  /* internal error (bad state)      */
 MV_StsNoMem=                   -4,  /* insufficient memory             */
 MV_StsBadArg=                  -5,  /* function arg/param is bad       */
 MV_StsBadFunc=                 -6,  /* unsupported function            */
 MV_StsNoConv=                  -7,  /* iter. didn't converge           */
 MV_StsAutoTrace=               -8,  /* tracing                         */
 MV_HeaderIsNull=               -9,  /* image header is NULL            */
 MV_BadImageSize=              -10, /* image size is invalid           */
 MV_BadOffset=                 -11, /* offset is invalid               */
 MV_BadDataPtr=                -12, /**/
 MV_BadStep=                   -13, /**/
 MV_BadModelOrChSeq=           -14, /**/
 MV_BadNumChannels=            -15, /**/
 MV_BadNumChannel1U=           -16, /**/
 MV_BadDepth=                  -17, /**/
 MV_BadAlphaChannel=           -18, /**/
 MV_BadOrder=                  -19, /**/
 MV_BadOrigin=                 -20, /**/
 MV_BadAlign=                  -21, /**/
 MV_BadCallBack=               -22, /**/
 MV_BadTileSize=               -23, /**/
 MV_BadCOI=                    -24, /**/
 MV_BadROISize=                -25, /**/
 MV_MaskIsTiled=               -26, /**/
 MV_StsNullPtr=                -27, /* null pointer */
 MV_StsVecLengthErr=           -28, /* incorrect vector length */
 MV_StsFilterStructContentErr= -29, /* incorr. filter structure content */
 MV_StsKernelStructContentErr= -30, /* incorr. transform kernel content */
 MV_StsFilterOffsetErr=        -31, /* incorrect filter ofset value */
 MV_StsBadSize=                -201, /* the input/output structure size is incorrect  */
 MV_StsDivByZero=              -202, /* division by zero */
 MV_StsInplaceNotSupported=    -203, /* in-place operation is not supported */
 MV_StsObjectNotFound=         -204, /* request can't be completed */
 MV_StsUnmatchedFormats=       -205, /* formats of input/output arrays differ */
 MV_StsBadFlag=                -206, /* flag is wrong or not supported */
 MV_StsBadPoint=               -207, /* bad CvPoint */
 MV_StsBadMask=                -208, /* bad format of mask (neither 8uC1 nor 8sC1)*/
 MV_StsUnmatchedSizes=         -209, /* sizes of input/output structures do not match */
 MV_StsUnsupportedFormat=      -210, /* the data format/type is not supported by the function*/
 MV_StsOutOfRange=             -211, /* some of parameters are out of range */
 MV_StsParseError=             -212, /* invalid syntax/structure of the parsed file */
 MV_StsNotImplemented=         -213, /* the requested function/feature is not implemented */
 MV_StsBadMemBlock=            -214, /* an allocated block has been corrupted */
 MV_StsAssert=                 -215, /* assertion failed */
 MV_GpuNotSupported=           -216,
 MV_GpuApiCallError=           -217,
 MV_OpenGlNotSupported=        -218,
 MV_OpenGlApiCallError=        -219
};







#endif  // _TYPES_H_

