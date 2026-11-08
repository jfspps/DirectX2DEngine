#include "main.h"
#include "rasteriser.h"

// see main.cpp
extern HWND mainWindowHandle;
extern int windowClosed;
extern DDSURFACEDESC2 ddsd;
extern LPDIRECTDRAWSURFACE7 lpddsBack;
extern LPDIRECTDRAWSURFACE7 lpddsPrimary;

int gameMain(void *params = NULL, int numParams = 0){
	// the main game loop

	if (windowClosed){
		// terminate game loop if window closed
		return (0);
	}

	// test if user is hitting ESC and send WM_CLOSE
	if (KEYDOWN(VK_ESCAPE)){
		PostMessage(mainWindowHandle, WM_CLOSE, 0, 0);
		windowClosed = 1;
	}

	// prep the back buffer
	DDRAW_INIT_STRUCT(ddsd);

	// lock the back buffer
	lpddsBack->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

	// set up an alias pointer to back buffer
	UCHAR *backBuffer = (UCHAR *) ddsd.lpSurface;

	// now clear the back buffer (flood with zeros)
	if (ddsd.lPitch == SCREEN_WIDTH){
		// linear memory
		memset(backBuffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
	} else {
		// non-linear memory

		UCHAR *destPtr = backBuffer;
		memset(destPtr, 0, SCREEN_WIDTH);

		destPtr += ddsd.lPitch;
	}

	// Demo C: draw polygons
	// define points of asteroid
	POLYGON2DI asteroids[64]; // the asteroids
	VERTEX2DI asteroidVertices[8] = {33,-3, 9,-18, -12,-9, -21,-12, -9,6, -15,15, -3,27, 21,21};

	// initialize all asteroids
	for (int curr_index = 0; curr_index < 64; curr_index++){
		// initialize the asteroid
		asteroids[curr_index].state       = 1;   // turn it on
		asteroids[curr_index].numVerts   = 8;  
		asteroids[curr_index].x0          = rand()%SCREEN_WIDTH;
		asteroids[curr_index].y0          = rand()%SCREEN_HEIGHT;
		asteroids[curr_index].xv          = -8 + rand()%17;
		asteroids[curr_index].yv          = -8 + rand()%17;
		asteroids[curr_index].colour       = rand()%256;
		asteroids[curr_index].vList       = new VERTEX2DI [asteroids[curr_index].numVerts];
	 
	   for (int index = 0; index < asteroids[curr_index].numVerts; index++)
			asteroids[curr_index].vList[index] = asteroidVertices[index];
	}

	for (int curr_index = 0; curr_index < 64; curr_index++)
    {
		// glow asteroids
		asteroids[curr_index].colour = rand()%256;

		// do the graphics
		draw2DIPolygon8Bit(
			&asteroids[curr_index], 
			(UCHAR *)ddsd.lpSurface, 
			ddsd.lPitch);
	}

	// begin drawing to the back buffer (this will
	// eventually be replaced by engine requirements)

	// Demo B: draw 10 random clipped lines
	//for (int index = 0; index < 10; index++){
	//	drawClipLine8bit(
	//		rand()%SCREEN_WIDTH, 
	//		rand()%SCREEN_HEIGHT,
	//		rand()%SCREEN_WIDTH, 
	//		rand()%SCREEN_HEIGHT,
	//		rand()%256,
	//		(UCHAR *)ddsd.lpSurface, 
	//		ddsd.lPitch);
 //   }


	// Demo A: plot 1000 random pixels
	//for (int index = 0; index < 1000; index++){
	//	int x = rand() % SCREEN_WIDTH;
	//	int y = rand() % SCREEN_HEIGHT;

	//	UCHAR colour = rand() % 256;
	//	backBuffer[x + y*ddsd.lPitch] = colour;
	//}

	// ...end of Demo

	if (FAILED(lpddsBack->Unlock(NULL))){
		// failed to unlock the back buffer
		return (0);
	}

	// flip the primary surface once, repeatedly until done
	while (FAILED(lpddsPrimary->Flip(NULL, DDFLIP_WAIT)));

	// milliseconds
	Sleep(500);

	return(1);
}

LPDIRECTDRAWCLIPPER attachClipper(
	LPDIRECTDRAW7 lpdd7, LPDIRECTDRAWSURFACE7 lpdds, int numRects, LPRECT clipList){

	int index;                         // looping var
	LPDIRECTDRAWCLIPPER lpddclipper;   // pointer to the newly created dd clipper
	LPRGNDATA regionData;             // pointer to the region data that contains
									// the header and clip list

	// first create the DirectDraw clipper
	if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, NULL)))
		return(NULL);

	// create the clip list....

	// first allocate memory for region data
	regionData = 
		(LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + (numRects * sizeof(RECT)));

	// now copy the rects into region data
	memcpy(regionData->Buffer, clipList, (numRects * sizeof(RECT)));

	// set up fields of header
	regionData->rdh.dwSize          = sizeof(RGNDATAHEADER);
	regionData->rdh.iType           = RDH_RECTANGLES;
	regionData->rdh.nCount          = numRects;
	regionData->rdh.nRgnSize        = numRects * sizeof(RECT);

	// set the bounding rectangle
	regionData->rdh.rcBound.left    =  64000;
	regionData->rdh.rcBound.top     =  64000;
	regionData->rdh.rcBound.right   = -64000;
	regionData->rdh.rcBound.bottom  = -64000;

	// find bounds of all clipping regions
	for (index = 0; index < numRects; index++){
		// test if the next rectangle unioned with the current bound is larger
		if (clipList[index].left < regionData->rdh.rcBound.left)
		regionData->rdh.rcBound.left = clipList[index].left;

		if (clipList[index].right > regionData->rdh.rcBound.right)
		regionData->rdh.rcBound.right = clipList[index].right;

		if (clipList[index].top < regionData->rdh.rcBound.top)
		regionData->rdh.rcBound.top = clipList[index].top;

		if (clipList[index].bottom > regionData->rdh.rcBound.bottom)
		regionData->rdh.rcBound.bottom = clipList[index].bottom;
	}

	// now we have computed the bounding rectangle region and set up the data
	// now let's set the clipping list

	if (FAILED(lpddclipper->SetClipList(regionData, 0))){
		// release memory and return error
		free(regionData);
		return(NULL);
	}

	// now attach the clipper to the surface
	if (FAILED(lpdds->SetClipper(lpddclipper))){
		// release memory and return error
		free(regionData);
		return(NULL);
	}

	// all is well, so release memory and send back the pointer to the new clipper
	free(regionData);
	return(lpddclipper);
}

int flipBitmap(UCHAR *image, int bytesPerLine, int height){

	UCHAR *buffer; // used to perform the image processing
	int index;

	// allocate the temporary buffer
	if (!(buffer = (UCHAR *)malloc(bytesPerLine * height)))
	   return(0);

	// copy image to work area
	memcpy(buffer, image, bytesPerLine * height);

	// flip vertically
	for (index = 0; index < height; index++)
		memcpy(&image[((height-1) - index) * bytesPerLine],
			   &buffer[index * bytesPerLine], 
			   bytesPerLine);

	free(buffer);

	// return success
	return(1);
}

int loadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename){

	int fileHandle, index;
	OFSTRUCT fileData;       // the file data information

	// open the file if it exists
	if ((fileHandle = OpenFile(filename, &fileData, OF_READ)) == -1)
	   return(0);

	// now load the bitmap file header
	_lread(fileHandle, &bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER));

	// test if this is a bitmap file
	if (bitmap->bitmapfileheader.bfType != BITMAP_ID){
	   _lclose(fileHandle);
	   return(0);
	}

	// first the bitmap infoheader

	// now load the bitmap file header
	_lread(fileHandle, &bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER));

	// now load the color palette if there is one
	if (bitmap->bitmapinfoheader.biBitCount == 8){
	   _lread(fileHandle, &bitmap->palette, MAX_COLORS_PALETTE * sizeof(PALETTEENTRY));

	   // now set all the flags in the palette correctly and fix the reversed 
	   // BGR RGBQUAD data format
	   for (index = 0; index < MAX_COLORS_PALETTE; index++){
		   // reverse the red and green fields
		   int tempColour = bitmap->palette[index].peRed;
		   bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
		   bitmap->palette[index].peBlue = tempColour;
	       
		   // always set the flags word to this
		   bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
	   }

	}

	// finally the image data itself
	_lseek(fileHandle, -(int)(bitmap->bitmapinfoheader.biSizeImage), SEEK_END);

	// now read in the image, if the image is 8 or 16 bit then simply read it
	// but if its 24 bit then read it into a temporary area and then convert
	// it to a 16 bit image

	if (bitmap->bitmapinfoheader.biBitCount == 8 || 
		bitmap->bitmapinfoheader.biBitCount == 16 || 
		bitmap->bitmapinfoheader.biBitCount == 24){

	   // delete the last image if there was one
	   if (bitmap->buffer)
		   free(bitmap->buffer);

	   // allocate the memory for the image
	   if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage))){
		  _lclose(fileHandle);
		  return(0);
		  }

	   // now read it in
	   _lread(fileHandle, bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage);

	} else {
	   // serious problem
	   return(0);
	}

	#if 0
	// write the file info out 
	printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d 
			\nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
			filename,
			bitmap->bitmapinfoheader.biSizeImage,
			bitmap->bitmapinfoheader.biWidth,
			bitmap->bitmapinfoheader.biHeight,
			bitmap->bitmapinfoheader.biBitCount,
			bitmap->bitmapinfoheader.biClrUsed,
			bitmap->bitmapinfoheader.biClrImportant);
	#endif

	// close the file
	_lclose(fileHandle);

	// flip the bitmap
	flipBitmap(bitmap->buffer, 
				bitmap->bitmapinfoheader.biWidth * 
					(bitmap->bitmapinfoheader.biBitCount/8), 
				bitmap->bitmapinfoheader.biHeight);

	// return success
	return(1);
}

int unloadBitmapFile(BITMAP_FILE_PTR bitmap){
	if (bitmap->buffer){
	   free(bitmap->buffer);
	   bitmap->buffer = NULL;
	}

	// return success
	return(1);
}

LPDIRECTDRAWSURFACE7 createOffscreenSurface(
	LPDIRECTDRAW7 lpdd7,
	int width,
	int height,
	int memTypeFlag){

	// pointer to the offscreen surface
	LPDIRECTDRAWSURFACE7 lpddsOff = NULL;

	// initialise the surface description
	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(ddsd));

	// start initialising the structure's fields

	// decide which valid fields will be provided or required
	// for the offscreen surface
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// we need to define the width and height of all offscreen surfaces
	// (this is usually set to handle whatever bitmap is stored, not
	// necessarily the entire viewport)
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	// now set the capabilities for an offscreen surface description, 
	// to occupy DDSCAPS_VIDEOMEMORY for video memory or
	// DDSCAPS_SYSTEMMEMORY for system memory
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | memTypeFlag;

	// now create the primary surface, and initialise lpddsprimary
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsOff, NULL))){
		// failed to build the primary surface, clean up and exit
		return(0);
	}

	// set the offscreen surface colour key to 0
	DDCOLORKEY colourKey;
	colourKey.dwColorSpaceLowValue = 0;
	colourKey.dwColorSpaceHighValue = 0;

	lpddsOff->SetColorKey(DDCKEY_SRCBLT, &colourKey);

	return lpddsOff;
}

LPDIRECTDRAWSURFACE7 createOffscreenSurfaceSrcBlitting(
	LPDIRECTDRAW7 lpdd7,
	int width,
	int height,
	int memTypeFlag,
	int lowColourKeyValue,
	int highColourKeyValue){

	// pointer to the offscreen surface
	LPDIRECTDRAWSURFACE7 lpddsOff = NULL;

	// initialise the surface description
	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(ddsd));

	// start initialising the structure's fields

	// decide which valid fields will be provided or required
	// for the offscreen surface (with source colour key blitting)
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT;

	// we need to define the width and height of all offscreen surfaces
	// (this is usually set to handle whatever bitmap is stored, not
	// necessarily the entire viewport)
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	// now set the capabilities for an offscreen surface description, 
	// to occupy DDSCAPS_VIDEOMEMORY for video memory or
	// DDSCAPS_SYSTEMMEMORY for system memory
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | memTypeFlag;

	// set the offscreen colour key DDSD_CKSRCBLT
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = lowColourKeyValue;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = highColourKeyValue;

	// now create the primary surface, and initialise lpddsprimary
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsOff, NULL))){
		// failed to build the primary surface, clean up and exit
		return(0);
	}

	// now set the colour key for surface itself, with source blitting
	DDCOLORKEY colourKey;
	colourKey.dwColorSpaceLowValue = lowColourKeyValue;
	colourKey.dwColorSpaceHighValue = highColourKeyValue;

	if (lowColourKeyValue < highColourKeyValue){
		// range of transparent colors required
		lpddsOff->SetColorKey(DDCKEY_SRCBLT | DDCKEY_COLORSPACE, &colourKey);
	} else {
		lpddsOff->SetColorKey(DDCKEY_SRCBLT, &colourKey);
	}

	return lpddsOff;
}

LPDIRECTDRAWSURFACE7 createOffscreenSurfaceDestBlitting(
	LPDIRECTDRAW7 lpdd7,
	int width,
	int height,
	int memTypeFlag,
	int lowColourKeyValue,
	int highColourKeyValue){

	// pointer to the offscreen surface
	LPDIRECTDRAWSURFACE7 lpddsOff = NULL;

	// initialise the surface description
	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(ddsd));

	// start initialising the structure's fields

	// decide which valid fields will be provided or required
	// for the offscreen surface (with source colour key blitting)
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKDESTBLT;

	// we need to define the width and height of all offscreen surfaces
	// (this is usually set to handle whatever bitmap is stored, not
	// necessarily the entire viewport)
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	// now set the capabilities for an offscreen surface description, 
	// to occupy DDSCAPS_VIDEOMEMORY for video memory or
	// DDSCAPS_SYSTEMMEMORY for system memory
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | memTypeFlag;

	// set the offscreen colour key DDSD_CKSRCBLT
	ddsd.ddckCKDestBlt.dwColorSpaceLowValue = lowColourKeyValue;
	ddsd.ddckCKDestBlt.dwColorSpaceHighValue = highColourKeyValue;

	// now create the primary surface, and initialise lpddsprimary
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsOff, NULL))){
		// failed to build the primary surface, clean up and exit
		return(0);
	}

	// now set the colour key for surface itself, with destination blitting
	DDCOLORKEY colourKey;
	colourKey.dwColorSpaceLowValue = lowColourKeyValue;
	colourKey.dwColorSpaceHighValue = highColourKeyValue;

	if (lowColourKeyValue < highColourKeyValue){
		// range of transparent colors required
		lpddsOff->SetColorKey(DDCKEY_DESTBLT | DDCKEY_COLORSPACE, &colourKey);
	} else {
		lpddsOff->SetColorKey(DDCKEY_DESTBLT, &colourKey);
	}

	return lpddsOff;
}