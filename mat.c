/*
* mat.c
*
* Matrix implement file
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

#define MV_MALLOC_ALIGN 0

/**********************************************************************\
*                               mat_t creation and basic operations                                     *
\**********************************************************************/

// Allocates underlying array data
void mvCreateData( mat_t* mat)
{
    size_t step, total_size;
    step = mat->step;

    if( mat->rows == 0 || mat->cols == 0 )
        return;

    if( mat->data.ptr != 0 )
        mvError(MV_StsError, "Data is already allocated" );

    if( step == 0 )
        step = MV_ELEM_SIZE(mat->type) * mat->cols;

    int64 _total_size = (int64)step*mat->rows + sizeof(int) + MV_MALLOC_ALIGN;
    total_size = (size_t)_total_size;
    if(_total_size != (int64)total_size)
        mvError(MV_StsNoMem, "Too big buffer is allocated" );
    mat->data.ptr = (u8*)mvAlloc((size_t)total_size);
}

// Assigns external data to array
void cvSetData( mat_t* mat, void* data, int step )
{
    mat->step = MV_ELEM_SIZE(mat->type) * mat->cols;
    mat->data.ptr = (u8*)data;
}


// Deallocates array's data
void cvReleaseData( mat_t* arr )
{
}

// Creates mat_t header only
mat_t* mvCreateMatHeader( int rows, int cols, int type )
{
    if( rows < 0 || cols <= 0 )
        mvError( MV_StsBadSize, "Non-positive width or height" );

    int min_step = MV_ELEM_SIZE(type)*cols;
    if( min_step <= 0 )
        mvError( MV_StsUnsupportedFormat, "Invalid matrix type" );

    mat_t* arr = (mat_t*)mvAlloc(sizeof(*arr));

    arr->step = min_step;
    arr->type = type;
    arr->rows = rows;
    arr->cols = cols;

    return arr;
}

// Creates mat_t and underlying data
mat_t* mvCreateMat( int height, int width, int type )
{
    mat_t* arr = (mat_t*)mvCreateMatHeader( height, width, type );
    mvCreateData( arr );

    return arr;
}

// Initializes mat_t header, allocated by the user
mat_t* mvInitMatHeader( mat_t* arr, int rows, int cols, int type, void* data)
{
    if( !arr )
        mvError( MV_StsNullPtr, "" );

    if( (u32)MV_ELEM_DEPTH(type) > MV_DEPTH_MAX )
        mvError( MV_BadNumChannels, "" );

    if( rows < 0 || cols <= 0 )
        mvError( MV_StsBadSize, "Non-positive cols or rows" );

    arr->type = type;
    arr->rows = rows;
    arr->cols = cols;
    arr->data.ptr = (u8*)data;
    arr->step = MV_ELEM_SIZE(type) * arr->cols;

    return arr;
}


// Deallocates the mat_t structure and underlying data
void mvReleaseMat( mat_t** array )
{
    if( !array )
        mvError( MV_HeaderIsNull, "" );
#if 0
    if( *array )
    {
        mat_t* arr = *array;

        if( !MV_IS_MAT_HDR_Z(arr) && !MV_IS_MATND_HDR(arr) )
            mvError( MV_StsBadFlag, "" );

        *array = 0;

        mvFree( &arr );
    }
#endif
}

// Creates a copy of matrix
mat_t*
mvCloneMat( const mat_t* src )
{
    if( !MV_IS_MAT_HDR( src ))
        mvError( MV_StsBadArg, "Bad mat_t header" );

    mat_t* dst = mvCreateMatHeader( src->rows, src->cols, src->type );

    if( src->data.ptr )
    {
        mvCreateData( dst );
        //mvCopy( src, dst );
    }

    return dst;
}
