/*
* bitmap_test.c
*
*/

#include <stdio.h>
#include "mv.h"
#include "image.h"

int main(int argc, char** argv)
{
    image_t *image = NULL;

    if (argc != 2) {
        printf("Usage: bitmap_test  <bitmap file name>\n");
        exit(-1);
    }

    image = mvLoadImage(argv[1], 1);
    if (image)
        mvShowImage("MV0.1 test", image);

    return 0;
}
