
#include "main.h"
#include "gameMain.h"
#include "gameInit.h"
#include "gameShutdown.h"

// GLOBALS ////////////////////////////////////////////////
HWND mainWindowHandle = NULL; // globally track main window
HINSTANCE hInstanceApp = NULL; // globally track hinstance

// directdraw stuff

LPDIRECTDRAW7         lpdd         = NULL;   // dd object
LPDIRECTDRAWSURFACE7  lpddsprimary = NULL;   // dd primary surface
LPDIRECTDRAWSURFACE7  lpddsback    = NULL;   // dd back surface
LPDIRECTDRAWPALETTE   lpddpal      = NULL;   // a pointer to the created dd palette
LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
PALETTEENTRY          palette[256];          // color palette
PALETTEENTRY          save_palette[256];     // used to save palettes
DDSURFACEDESC2        ddsd;                  // a direct draw surface description struct
DDBLTFX               ddbltfx;               // used to fill
DDSCAPS2              ddscaps;               // a direct draw surface capabilities struct
HRESULT               ddrval;                // result back from dd calls
DWORD                 start_clock_count = 0; // used for timing

// these defined the general clipping rectangle
int min_clip_x = 0,                          // clipping rectangle 
    max_clip_x = SCREEN_WIDTH-1,
    min_clip_y = 0,
    max_clip_y = SCREEN_HEIGHT-1;

// these are overwritten globally by DD_Init()
int screen_width  = SCREEN_WIDTH,            // width of screen
    screen_height = SCREEN_HEIGHT,           // height of screen
    screen_bpp    = SCREEN_BPP;              // bits per pixel

char mainLogBuffer[80];                     // general printing buffer


LRESULT CALLBACK WindowProc(HWND hWnd, 
						    UINT msg, 
                            WPARAM wParam, 
                            LPARAM lParam){

	// for WM_PAINT messages
	PAINTSTRUCT	ps;
	HDC	hdc;
	char logBuffer[80];

	switch(msg)
		{	
		case WM_CREATE: 
			{
			// do initialization stuff here

			return(0);
			} break;
	   
		case WM_PAINT: 
			{
			// simply validate the window 
   			hdc = BeginPaint(hWnd, &ps);	 
	        
			// end painting
			EndPaint(hWnd, &ps);

			return(0);
   			} break;

		case WM_DESTROY: 
			{

			// kill the application, sending a WM_QUIT message 
			PostQuitMessage(0);

			return(0);
			} break;

		default:break;

		}

	// process any unhandled messages
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpcmdline,
					int ncmdshow){

	// holds the class created
	WNDCLASSEX winclass;
	// generic window handle
	HWND hWnd;
	// generic message
	MSG msg;
	// graphics device context
	HDC hdc;

	// initialise the window class stucture
	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
							  CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hInstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	// save hinstance in global
	hInstanceApp = hInstance;

	// register the window class
	if (!RegisterClassEx(&winclass))
		return(0);

	// create the window
	if (!(hWnd = CreateWindowEx(NULL,                  
								WINDOW_CLASS_NAME,     
								"DirectX 9.0c 3D Game engine (ESC to close)",
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					 			0,0,	  // initial (x,y)
								900,600,  // initial (width, height)
								NULL,	  // handle to parent 
								NULL,	  // handle to menu
								hInstance, // instance of this application
								NULL)))	// extra creation parms
	return(0);

	// save main window handle
	mainWindowHandle = hWnd;

	// initialize game engine
	gameInit(NULL, NULL);

	while(TRUE){
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		   if (msg.message == WM_QUIT)
			   break;
		
		   // translate any accelerator keys
		   TranslateMessage(&msg);

		   // send the message to the window proc
		   DispatchMessage(&msg);
		   }
	    
		   // main game processing
		   gameMain(NULL, NULL);
	}

	gameShutdown(NULL, NULL);

	// return to Windows like this
	return (msg.wParam);
}
