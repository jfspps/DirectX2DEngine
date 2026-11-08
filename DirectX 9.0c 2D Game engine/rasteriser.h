#pragma once

/*
*Draws a line between two endpoints at (x0, y0) and (x1, y1), 8-bit mode only
*/
int drawLine8bit(int x0, int y0, int x1, int y1, 
				 UCHAR colour, UCHAR *videoBuffer, int lpitch);

/*
*Draws a line between two endpoints at (x0, y0) and (x1, y1), 16-bit mode only
*/
int drawLine16bit(int x0, int y0, int x1, int y1,
				  USHORT colour, UCHAR *videoBuffer, int lpitch);

/*
* Clips a line by updating the endpoints (x1, y1) and (x2, y2)
*/
int clipLine(int &x1, int &y1, int &x2, int &y2);

/*
* Draws a clipped line starting from the endpoints (x1, y1) and (x2, y2)
*/
int drawClipLine8bit(int x0,int y0, int x1, int y1, 
				 UCHAR color, UCHAR *videoBuffer, int lpitch);