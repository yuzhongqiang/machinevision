/*
* bmp.c
*
* Bitmap image file process
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

/* Bitmap file structure */
/*
        +---------------------------
        +                                          +
        +    File header 14bytes           +
        +                                          +
        +---------------------------
        +                                          +
        +  bitmap info header 40bytes  +
        +                                          +
        +---------------------------
        +                                          +
        +  plane information(optional)  +
        +                                          +
        +---------------------------
        +                                          +
        +              data area               +
        +                                          +
        +---------------------------
*/

/* Bitmap file header */
typedef struct file_header_t {
    u16 Flag; //Bitmap file flag: "BM"
    u32 FileSize; //file size
    u16 Reserved1;  //0x00 0x00
    u16 Reserved2;  //0x00 0x00
    u32 OffBits;  //offset of image data area
}__attribute__((packed)) file_header_t;

/* Bitmap info header */
typedef struct info_header_t {
    u32 Size;    //size of this structure, usually 0x28 bytes
    u32 Width;    //image width
    u32 Height;   //image height
    u16 Planes;   //planes count, always 1
    u16 BitPerPels; //bits count per pixel
    u32 Compression;  //compression type: 
                      /* 0: no compress;x
			      1: RLE 8 compression(BI_RLE8)
			      2: RLE 4 compression(BI_RLE4)
			      3: Bitfields(BI_BITFIELDS)  */
    u32 DataSize;    //size of data area
    u32 XPelsPerMeter; //horizontal pixels per meter
    u32 YPelsPerMeter; //vertical pixels per meter
    u32 ClrUsed;      //color used
    u32 ClrImportant;
}info_header_t __attribute__((aligned(1)));

/* Color plane (optional) */
typedef struct rgb_plate_t {
    u8 Blue;
    u8 Green;
    u8 Red;
    u8 Reserved;
}rgb_plate_t;

/* Bitmap data */
typedef struct rgb_pixel_t {
    u8 red;
    u8 Green;
    u8 Blue;
}rgb_pixel_t;

MV_STATIC void mat_1b2u8(u8* data, int row, u32 width, mat_t *mat)
{
    int i = 7, w = 0;
    u8 mask;
    u8* ptr = mat->data.ptr + row * mat->step;

    for (; w<width; w++) {
        mask = 0x01 << i;
        i--;
        if (i<0)
            i = 7;
        ptr[w] = data[w/8] & mask ? 1 : 0;
    }
}

MV_STATIC void mat_24b2s32(u8* data, int row, u32 width, mat_t *mat)
{
    int i;
    
    /* in data, the color order is {red, green, blue} */
    u8* ptr = mat->data.ptr + row * mat->step;
    for (i=0; i<width; i++) {
        ptr[0] = data[i*3+2];
        ptr[1] = data[i*3+1];
        ptr[2] = data[i*3];
        ptr += 3;
    }
}

image_t* bmpLoadImage(const char* filename, int iscolor)
{
    int i, ret = 0;
    int row;
    int plate_len = 0;
    int line_bytes;
    file_header_t hdr;
    info_header_t ihdr;
    mat_t *mat = NULL;

    rgb_plate_t* plate = NULL;
    image_t *image = NULL;
    u8 *data;
    FILE *fp;

    fp = fopen(filename, "r");
    if (NULL == fp)
        return NULL;

    if (fread(&hdr, sizeof(file_header_t), 1, fp) != 1)
        goto err1;
    if (hdr.Flag != 0x4d42)  //"BM"  Little-endian
        goto err1;

    if (fread(&ihdr, sizeof(info_header_t), 1, fp) != 1)
        goto err1;
    if (ihdr.Size != sizeof(ihdr))
        goto err1;
        
    /* get color plate */
    plate_len = hdr.OffBits - sizeof(file_header_t) - sizeof(info_header_t);
    if (plate_len > 0) {
        plate = (rgb_plate_t*)mvAlloc(plate_len);
        if (NULL == plate)
            goto err1;

        if (fread(plate, sizeof(rgb_plate_t), plate_len/sizeof(rgb_plate_t), fp)
                != plate_len/sizeof(rgb_plate_t)) {
            goto err2;
        }
    }

    /* now file pointer to the data area */
    line_bytes = (ihdr.BitPerPels * ihdr.Width + 7) / 8;
    line_bytes = (line_bytes + 3) / 4 *4;
    data = (u8*)mvAlloc(line_bytes);
    if (NULL == data)
        goto err2;

    /* create image and matrix */
    image = mvCreateImageHeader(ihdr.Width, ihdr.Height, 3, IMG_DEPTH_8U);
    image->img_size = ihdr.Width * ihdr.Height;
    if (NULL == image)
        goto err1;
    mat = mvCreateMat(ihdr.Height, ihdr.Width, MAT_8UC3);
    image->data = mat;    

    /* parse data */
    row = ihdr.Height - 1;
    while (fread(data, line_bytes, 1, fp) == 1) {
        switch (ihdr.BitPerPels) {
            case 1:
                mat_1b2u8(data, row, ihdr.Width, mat);
                break;

            case 2:
                break;

            case 4:
                break;

            case 8:
                break;

            case 15:
                break;
                
            case 16:
                break;
                
            case 24:
                mat_24b2s32(data, row, ihdr.Width, mat);
                break;
                
            case 32:
                break;
                
            default:
                break;
        }

        if (--row < 0)
            break;
    }

err3:
    free(data);
err2:
    free(plate);    
err1:
    fclose(fp);
    
    return image;    
}













