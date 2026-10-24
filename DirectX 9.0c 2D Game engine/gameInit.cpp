#include "main.h"

// IID_IDirectDraw7: make sure DXGUID.LIB is also present in the project dir
// or linked via Project settings

// see main.cpp
extern LPDIRECTDRAW7 lpdd;
extern HWND mainWindowHandle;
extern DDSURFACEDESC2 ddsd;
extern LPDIRECTDRAWSURFACE7 lpddsPrimary;
extern LPDIRECTDRAWSURFACE7 lpddsBack;
extern LPDIRECTDRAWPALETTE lpddPalette;
extern PALETTEENTRY palette[256];

int gameInit(void *params = NULL, int numParams = 0){
	// this is called once after the initial window is created and
	// before the main event loop is entered, do all your initialization
	// here

	// create IDirectDraw interface 7.0 object and test for error
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
	   return(0);

	// set cooperation to fullscreen
	if(FAILED(lpdd->SetCooperativeLevel(
		mainWindowHandle, 
		DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT))){
			// failed to set cooperative level with Windows
			return (0);
	}

	if (FAILED(lpdd->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0))){
		// failed to set display mode
		return (0);
	}

	// flood the surface struct with zeros
	DDRAW_INIT_STRUCT(ddsd);

	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

	// prep for one back buffer
	ddsd.dwBackBufferCount = 1;

	// request a complex, flippable surface
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;

	// create the primary surface
	if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsPrimary, NULL))){
		// failed to create primary surface
		return (0);
	}

	// now query for attached surface (back buffer) from
	// primary surface

	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	// repeatedly attempt request
	while (FAILED(lpddsPrimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsBack)));

	// set up the palette for 8-bit mode
	for (int colour = 1; colour < 255; colour++){
		palette[colour].peRed = rand() % 256;
		palette[colour].peGreen = rand() % 256;
		palette[colour].peBlue = rand() % 256;

		// this is needed to prevent Windows or DirectX
		// automatically optimising the palette
		palette[colour].peFlags = PC_NOCOLLAPSE;
	}

	// black
	palette[0].peRed = 0;
	palette[0].peGreen = 0;
	palette[0].peBlue = 0;
	palette[0].peFlags = PC_NOCOLLAPSE;

	// white
	palette[255].peRed = 0;
	palette[255].peGreen = 0;
	palette[255].peBlue = 0;
	palette[255].peFlags = PC_NOCOLLAPSE;

	if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE,
		palette, &lpddPalette, NULL))){
			// failed to create palette
			return (0);
	}

	// attach palette to primary surface
	if (FAILED(lpddsPrimary->SetPalette(lpddPalette))){
		// failed to attach palette
		return (0);
	}

	return(1);
}
