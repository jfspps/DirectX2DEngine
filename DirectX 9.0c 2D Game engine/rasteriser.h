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

/*
* Draws a polygon in 8-bit mode, I for int (x, y)
*/
int draw2DIPolygon8Bit(POLYGON2DI_PTR polygon, UCHAR *videoBuffer, int lPitch);

/*
* Draws a polygon in 8-bit mode, F for float (x, y)
*/
int draw2DFPolygon8Bit(POLYGON2DF_PTR polygon, UCHAR *videoBuffer, int lPitch);

/*
* Rotates a 2D (assumed float based) polygon by theta degrees
*/
int rotatePolygon2D(POLYGON2DF_PTR poly, int thetaDeg);

/*
* Scales a 2D (assumed float based) polygon by sx > 0 and sy > 0
*/
int scalePolygon2DF(POLYGON2DF_PTR polygon, float sx, float sy);

/*
* Scales a 2D (assumed int based) polygon by sx > 0 and sy > 0
*/
int scalePolygon2DI(POLYGON2DI_PTR polygon, float sx, float sy);
