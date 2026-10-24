#include "main.h"

// see main.cpp
extern LPDIRECTDRAW7 lpdd;
extern LPDIRECTDRAWPALETTE lpddPalette;
extern LPDIRECTDRAWSURFACE7 lpddsPrimary;
extern LPDIRECTDRAWSURFACE7 lpddsBack;

int gameShutdown(void *params = NULL, int numParams = 0){
	// this is called after the game is exited and the main event
	// loop while is exited, do all you cleanup and shutdown here

	// first the palette
	if (lpddPalette){
	   lpddPalette->Release();
	   lpddPalette = NULL;
	}

	// now the back buffer surface
	if (lpddsBack){
	   lpddsBack->Release();
	   lpddsBack = NULL;
	}

	// now the primary surface
	if (lpddsPrimary){
	   lpddsPrimary->Release();
	   lpddsPrimary = NULL;
	}

	// now release the IDirectDraw7 interface
	if (lpdd){
	   lpdd->Release();
	   lpdd = NULL;
	}

	// return success or failure or your own return code here
	return(1);
}