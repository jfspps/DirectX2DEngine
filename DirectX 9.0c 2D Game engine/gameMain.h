#pragma once

/* 
* Defines the game loop
*/
int gameMain(void *params, int numParams);

/* 
* Takes a clipping list and returns a clipper attached to the surface
*/
LPDIRECTDRAWCLIPPER attachClipper(
	LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, int numRects, LPRECT clipList);

int flipBitmap(UCHAR *image, int bytesPerLine, int height);

int loadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename);

int unloadBitmapFile(BITMAP_FILE_PTR bitmap);

LPDIRECTDRAWSURFACE7 createOffscreenSurface(LPDIRECTDRAW7 lpdd7, int width, int height, int memTypeFlag);

LPDIRECTDRAWSURFACE7 createOffscreenSurfaceSrcBlitting(
	LPDIRECTDRAW7 lpdd7, int width, int height, int memTypeFlag, int lowColourKeyValue, int highColourKeyValue);

LPDIRECTDRAWSURFACE7 createOffscreenSurfaceDestBlitting(
	LPDIRECTDRAW7 lpdd7, int width, int height, int memTypeFlag, int lowColourKeyValue, int highColourKeyValue);