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