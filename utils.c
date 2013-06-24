/*
* utils.c
*
* utility file
*/

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

void* mvAlloc(int size)
{
    return malloc(size);
}

void mvFree(void* buf)
{
    free(buf);
}

void mvError(int code, const char* msg)
{

}

void mvCopy(void *dest, void* src, size_t n)
{
    memcpy(dest, src, n);
}
