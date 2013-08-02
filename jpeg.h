/*
* jpeg.h
*
*/

#ifndef _JPEG_H_
#define _JPEG_H_

typedef enum {			/* JPEG marker codes */
  M_SOF0  = 0xc0,         /* Start Of Frame */
  M_SOF1  = 0xc1,  /* Start Of Frame */
 
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  
  M_DHT   = 0xc4,
  
  M_DAC   = 0xcc,
  
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  
  M_SOI   = 0xd8,    /* Start Of Image */
  M_EOI   = 0xd9,    /* End Of Image */
  M_SOS   = 0xda,   /* Start Of Scan */
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  
  M_TEM   = 0x01,
  
  M_ERROR = 0x100
} JPEG_MARKER;

typedef struct seg_header_t {
	u8 flag;    // must be 0xFF
	u8 id;      // segment ID
}seg_header_t;

typedef struct seg_app0_t {
	seg_header_t hdr;  // 0xFF 0xE0
	u16 length;        // length include this "length" field + content
					   //0x10 or 16+3n
	char[5] jfif;        //"JFIF\0"
	u8  major;
	u8  minor;
	u16 xdensity;
	u16 ydensity;
	u8  preview[0];
}seg_app0_t;

/* some jpeg file don't have this segment */
typedef struct seg_com_t {
	seg_header_t hdr;  // 0xFF 0xFE
	u16 length;
	char comment[0];
}seg_com_t;




























	

#endif   //_JPEG_H_
