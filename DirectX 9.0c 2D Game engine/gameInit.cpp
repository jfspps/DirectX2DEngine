#include "main.h"

// IID_IDirectDraw7: make sure DXGUID.LIB is also present in the project dir
// or linked via Project settings

extern LPDIRECTDRAW7 lpdd;
extern HWND mainWindowHandle;

int gameInit(void *params = NULL, int numParams = 0){
	// this is called once after the initial window is created and
	// before the main event loop is entered, do all your initialization
	// here

	// create IDirectDraw interface 7.0 object and test for error
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
	   return(0);

	// set cooperation to normal since this will be a windowed app
	lpdd->SetCooperativeLevel(
		mainWindowHandle, 
		DDSCL_NORMAL);

	// return success or failure or your own return code here
	return(1);
}
