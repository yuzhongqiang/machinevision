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

void mvError(const char* msg)
{

}
