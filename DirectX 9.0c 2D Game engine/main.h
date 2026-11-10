#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h> 

// should also add WINMM.LIB as linker dependency 
#include <mmsystem.h>

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

// copied from the DirectX SDK to the parent directory of this project;
// also include DDRAW.LIB from the DirectX SDK with this project
#include "ddraw.h"

#define WINDOW_CLASS_NAME "WINCLASS1"

// MACROS /////////////////////////////////////////////////
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      8    // bits per pixel
#define MAX_COLORS      256  // maximum colors

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define MAX_COLORS_PALETTE   256

typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

// MACROS /////////////////////////////////////////////////

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// initializes a DirectDraw struct
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct, 0, sizeof(ddstruct)); ddstruct.dwSize = sizeof(ddstruct);}

// container structure for bitmaps .BMP file
typedef struct BITMAP_FILE_TAG{
	BITMAPFILEHEADER bitmapfileheader;  // bitmap file header
    BITMAPINFOHEADER bitmapinfoheader;  // bitmap file info (inc. palette if necessary)
    PALETTEENTRY     palette[256];
    UCHAR            *buffer;           // pointer to the bitmap data
} BITMAP_FILE, *BITMAP_FILE_PTR;

// 2D polygon vertex, I for int (x, y)
typedef struct VERTEX2DI_TYP {
	int x, y;
} VERTEX2DI, *VERTEX2DI_PTR;

// 2D polygon, I for int (x, y)
typedef struct POLYGON2DI_TYP {
	int state;
	int numVerts;
	int x0, y0;
	int xv, yv;
	DWORD colour;
	VERTEX2DI *vList;
} POLYGON2DI, *POLYGON2DI_PTR;

// 2D polygon vertex, F for float (x, y)
typedef struct VERTEX2DF_TYP {
	float x, y;
} VERTEX2DF, *VERTEX2DF_PTR;

// 2D polygon, F for float (x, y)
typedef struct POLYGON2DF_TYP {
	int state;
	int numVerts;
	float x0, y0;
	float xv, yv;
	DWORD colour;
	VERTEX2DF *vList;
} POLYGON2DF, *POLYGON2DF_PTR;
