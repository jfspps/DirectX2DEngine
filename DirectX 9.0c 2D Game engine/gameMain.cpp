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