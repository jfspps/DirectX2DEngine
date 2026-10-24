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