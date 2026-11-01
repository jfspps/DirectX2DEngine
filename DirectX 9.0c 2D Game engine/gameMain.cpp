#include "main.h"

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

	// begin drawing to the back buffer;
	// in this example we plot 5000 random pixels
	// but later would be updated for the engine requirements

	for (int index = 0; index < 5000; index++){
		int x = rand() % SCREEN_WIDTH;
		int y = rand() % SCREEN_HEIGHT;

		UCHAR colour = rand() % 256;
		backBuffer[x + y*ddsd.lPitch] = colour;
	}

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
