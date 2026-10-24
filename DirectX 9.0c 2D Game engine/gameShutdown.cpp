#include "main.h"

extern LPDIRECTDRAW7 lpdd;

int gameShutdown(void *params = NULL, int numParams = 0){
	// this is called after the game is exited and the main event
	// loop while is exited, do all you cleanup and shutdown here

	// simply blow away the IDirectDraw7 interface
	if (lpdd){
	   lpdd->Release();
	   lpdd = NULL;
	}

	// return success or failure or your own return code here
	return(1);
}