/*
* bmp.c
*
* Bitmap image file process
*/

#include <stdio.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char  s8;
typedef short s16;
typedef int s32;


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
typedef struct {
    u16 Flag; //Bitmap file flag: "BM"
    u32 FileSize; //file size
    u16 Reserved1;  //0x00 0x00
    u16 Reserved2;  //0x00 0x00
    u32 OffBits;  //offset of image data area
}file_header_t;

/* Bitmap info header */
typedef struct {
    u32 Size;    //size of this structure, usually 0x28 bytes
    u32 Width;    //image width
    u32 Height;   //image height
    u16 Plates;   //planes count, always 1
    u16 BitPerPels; //bits count per pixel
    u32 Compression;  //compression type: 
                          //0-no compress; 1-8bits compression; 2:4bits compression
    u32 DataSize;    //size of data area
    u32 XPelsPerMeter; //horizontal pixels per meter
    u32 YPelsPerMeter; //vertical pixels per meter
    u32 ClrUsed;      //color used
    u32 ClrImportant;
}info_header_t;

/* Color plane (optional) */
typedef struct {
    u8 Blue;
    u8 Green;
    u8 Red;
    u8 Reserved;
}rgb_plate_t;

/* Bitmap data */
typedef struct {
    u8 red;
    u8 Green;
    u8 Blue;
}image_data_t;

image_t* bmpLoadImage(const char* filename, int iscolor)
{
    int i, ret = NULL;
    int row;
    int plate_len = 0;
    int line_bytes;
    file_header_t hdr;
    info_header_t ihdr;
    rgb_plate_t* plate = NULL;
    image_data_t data;
    image_t *image = NULL;
    u8 *data;
    FILE *fp;

    fp = fopen(filename, "r");
    if (NULL == fp)
        return NULL;

    if (fread(&hdr, sizeof(file_header_t), 1, fd) != 1)
        goto err1;
    if (hdr.Flag != 0x424d)  //"BM"
        goto err1;

    if (fread(&ihdr, sizeof(info_header_t), 1, fd) != 1)
        return goto err1;
    if (ihdr.Size != sizeof(ihdr))
        goto err1;
        
    image = mvCreateImageHeader(ihdr.Width, ihdr.Height, ihdr.Plates);
    if (NULL == image)
        goto err1;

    /* get color plate */
    plate_len = hdr.OffBits - sizeof(file_header_t) - sizeof(info_header_t);
    if (plate_len > 0) {
        plate = (rgb_plate_t*)malloc(plate_len);
        if (NULL == plate)
            goto err1;

        if (fread(plate, sizeof(rgb_plate_t), plate_len/sizeof(rgb_plate_t), fp)
                != plate_len/sizeof(rgb_plate_t)) {
            goto err2;
        }
    }

    /* now file pointer to the data area */
    line_bytes = (ihdr.BitPerPels * ihdr.Width + 7) / 8;
    data = (u8*)malloc(line_bytes);
    if (NULL == data)
        goto err2;

    row = ihdr.Height;
    while (fread(data, line_bytes, 1, fp) == 1) {
        row--;
        switch (ihdr.BitPerPels) {
            case 1:
                for (i=0; i<ihdr.Width; i++) {
                    image->data
                }
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
                break;
                
            case 32:
                break;
                
            default:
                break;
        }

    }
    

    
err3:
    free(data);
err2:
    free(plate);    
err1:
    fclose(fp);
    
    return image;    
}












