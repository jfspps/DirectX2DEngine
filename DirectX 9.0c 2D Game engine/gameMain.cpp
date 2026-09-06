#include "main.h"

// see main.cpp
extern HWND mainWindowHandle;

int gameMain(void *params = NULL, int numParams = 0){

	// test if user is hitting ESC and send WM_CLOSE
	if (KEYDOWN(VK_ESCAPE))
	   SendMessage(mainWindowHandle, WM_CLOSE, 0, 0);

	return(1);
}