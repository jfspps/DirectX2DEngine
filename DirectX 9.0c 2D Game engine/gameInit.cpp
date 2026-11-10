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

const double PI = 3.1415926535;

float cosLookup[360];
float sinLookup[360];

// these can be removed after Demos are no longer required
extern POLYGON2DF asteroidsF[64]; // the asteroids (float)
extern POLYGON2DI asteroidsI[64]; // the asteroids (int)

// prepare 2D int based polygons
void preparePolygons2DI(POLYGON2DI_PTR asteroids){
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
}

// prepare 2D float based polygons
void preparePolygons2DF(POLYGON2DF_PTR asteroids){
	VERTEX2DF asteroidVertices[8] = {33,-3, 9,-18, -12,-9, -21,-12, -9,6, -15,15, -3,27, 21,21};

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
		asteroids[curr_index].vList       = new VERTEX2DF [asteroids[curr_index].numVerts];
	 
	   for (int index = 0; index < asteroids[curr_index].numVerts; index++)
			asteroids[curr_index].vList[index] = asteroidVertices[index];
	}
}


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

	for (int ang = 0; ang < 360; ang++){
		// convert ang to radians
		float thetaRad = (float)ang*PI/(float)180;

		// insert next entry into table
		cosLookup[ang] = cos(thetaRad);
		sinLookup[ang] = sin(thetaRad);
	}

	// this prepares asteroids and can be removed after Demos are no longer required
	preparePolygons2DF(asteroidsF); // demo D
	preparePolygons2DI(asteroidsI); // demo C

	return(1);
}
