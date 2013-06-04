/*
* bmp.c
*
* Bitmap image file process
*/

#include <stdio.h>

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
typedef struct tagBMPFILEHEADER {
    unsigned short bfType; //Bitmap file flag: "BM"
    unsigned short bfSize; //file size
    WORD bfReserved1;  //0x00 0x00
    WORD bfReserved2;  //0x00 0x00
    unsigned long bfOffBits;  //offset of data area
}BMPHEADER;

/* Bitmap info header */
typedef struct tagBMPINFOHEADER {
    DWORD biSize;    //size of this structure, usually 0x28 bytes
    LONG biWidth;    //image width
    LONG biHeight;   //image height
    WORD biPlanes;   //planes count, always 1
    WORD biBitCount; //bits count of pixel
    DWORD biCompression;  //compression type: 
                          //0-no compress; 1-8bits compression; 2:4bits compression
    DWORD biSizeImage;    //size of data area
    LONG biXPelsPerMeter; //horizontal pixels per meter
    LONG biYPelsPerMeter; //vertical pixels per meter
    DWORD biClrUsed;      //color used
    DWORD biClrImportant;
}BMPINFOHEADER;

/* Color plane (optional) */
typedef struct tagBMPRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
}BMPRGBQUAD;

/* Bitmap data */
typedef struct tagIMAGEDATA {
    BYTE red;
    BYTE green;
    BYTE blue;
}IMAGEDATA;




image_t* bmpLoadImage(const char* filename, int iscolor)
{
	image_t image;

	memset(&image, 0, sizeof(image_t));







	
}













