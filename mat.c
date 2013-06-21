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
    int total_size = mat->step*mat->rows + MV_MALLOC_ALIGN;
    u8* buf = NULL;

    buf = (u8*)malloc(total_size);
    mat->data.ptr = buf;
    //mat->data.ptr = (u8*)mvAlloc(total_size);
    if (!mat->data.ptr)
        mvError(ERR_NOMEM, "Too big buffer to allocated");
}

// Deallocates array's data
void mvReleaseData( mat_t* mat )
{
    free(mat->data.ptr);
}

// Creates mat_t header only
mat_t* mvCreateMatHeader( int rows, int cols, int type )
{
    mat_t* mat = (mat_t*)mvAlloc(sizeof(mat_t));
    mat->step = MAT_ELEM_SIZE(type)*cols;;
    mat->type = type;
    mat->rows = rows;
    mat->cols = cols;

    return mat;
}

// Creates mat_t and underlying data
mat_t* mvCreateMat( int height, int width, int type )
{
    mat_t* mat = (mat_t*)mvCreateMatHeader( height, width, type );
    mvCreateData(mat);

    return mat;
}

// Deallocates the mat_t structure and underlying data
void mvReleaseMat( mat_t* mat )
{
    if( !mat )
        mvError( ERR_NULLPTR, "" );

    mvReleaseData(mat);
}

// Creates a copy of matrix
mat_t* mvCloneMat( const mat_t* src )
{
    mat_t* dst = mvCreateMatHeader( src->rows, src->cols, src->type );

    if( src->data.ptr )
    {
        mvCreateData( dst );
        mvCopy( src, dst );
    }

    return dst;
}
