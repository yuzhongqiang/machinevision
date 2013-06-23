/*
* wnd_gtk.c
*
*/

#include "gtk/gtk.h"
#include <stdio.h>
#include <commctrl.h>
#incldue <winuser.h>
#include "types.h"

static void FillBitmapInfo(BITMAPINFO *bmi, int width, int height, int bpp, int origin)
{
		BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
		
		memset(bmih, 0, sizeof(*bmih));
		bmih->biSize = sizeof(BITMAPINFOHEADER);
		bmih->biWidth = width;
		bmih->biHeight = origin ? abs(height) : -abs(height);
		bmih->biPlanes = 1;
		bmih->biCompression = BI_RGB;
		
		if (bpp == 8) {
				RGBQUAD* palette = bmi->bmiColors;
				int i;
				for (i=0; i<256; i++) {
						palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (u8)i;
						palette[i].rgbReserved = 0;
				}
		}
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		break;
		
	case 
	}
}

/* Of pixbuf, color order is {blue, green, red} */
void mvShowImage(const char *name, image_t *image)
{
    static int initialized = 0;
    unsigned char *data = NULL;
    mat_t *mat = (mat_t*)(image->data);
    
    if (!initialized) {
			WNDCLASS wc;
			wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
			wc.lpfnWndProc = WindowProc;
			wc.cbClsExtra = 0;
			wc.hInstance = 0;
			wc.lpszClassName = "MV class";
			wc.lpszMenuName = "MV Menu";
			wc.hIcon = LoadIcon(0, IDI_APPLICATION);
			wc.hCursor = (HCURSOR)LoadCursor(0, (LPSTR)(size_t)IDC_CROSS);
			wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
			RegisterClass(&wc);

			initialized = 1;
    }
    
    DWORD defStyle = WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		HWND hWnd, mainhWnd;
		mainhWnd = CreateWindow("Main MV class", name, defStyle | WS_OVERLAPPED,
								0, 0, 320, 320,	0, 0, 0, 0);								
		if (!mainhWnd)
			MV_ERROR(ERR_NOMEM, "Frame window can not be created");			
		ShowWindow(mainhWnd, SW_SHOW);
		
		hWnd = CreateWindow("MV class", "", (defStype & ~WS_SIZEBOX) | WS_CHILD,
							CW_USEDEFAULT, 0, 280, 280, mainhWnd, 0, 0, 0);
		if (!hWnd)
			MV_ERROR(ERR_NOMEM, "Frame window can not be created");
		ShowWindow(hWnd, SW_SHOW);
		
		HDC dc = CreateCompatibleDC(0);
		HGDIOBJ hImage;
		u8 buffer[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];
		BITMAPINFO *binfo = (BITMAPINFO*)buffer;
		DeleteObject(SelectObject(dc, hImage));
		hImage = 0;
		
		void * dst_ptr = 0;
		FillBitmapInfo(binfo, mat->cols, mat->rows, mat->chn*8, 1);
		hImage = SelectObject(dc, CreateDIBSection(dc, binfo, DIB_RGB_COLORS, &dst_ptr, 0, 0));
		
		InvalidateRect(hwnd, 0, 0);
}

