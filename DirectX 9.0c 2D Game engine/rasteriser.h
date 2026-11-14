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
* Translates a polygon (assumed int based) by some displacement dx (to the left)
* and displacement dy (down)
*/
int translatePolygon2DI(POLYGON2DI_PTR polygon, int dx, int dy);

/*
* Translates a polygon (assumed float based) by some displacement dx (to the left)
* and displacement dy (down)
*/
int translatePolygon2DF(POLYGON2DF_PTR polygon, int dx, int dy);

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

/*
* Fills a 3x2 matrix with the sent data in row major form
*/
int matrixInit3X3(MATRIX3X3_PTR ma,
				  float m00, float m01, float m02,
				  float m10, float m11, float m12,
				  float m20, float m21, float m22);

/*
* Multiplies a 1x3 matrix ma against a 3x3 matrix mb, before updating the result as mprod
*/
int matrixMultiply1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod);

/*
* Translates a polygon (assumed int based) by some displacement dx (to the left)
* and displacement dy (down) with matrices
*/
int translatePolygon2DIM(POLYGON2DI_PTR polygon, int dx, int dy);

/*
* Translates a polygon (assumed float based) by some displacement dx (to the left)
* and displacement dy (down) with matrices
*/
int translatePolygon2DFM(POLYGON2DF_PTR polygon, int dx, int dy);

/*
* Rotates the polygon (assumed int based) about the centre coordinate by some angle theta (degrees) with matrices
*/
int rotatePolygon2DIM(POLYGON2DI_PTR polygon, int thetaDeg);

/*
* Rotates the polygon (assumed float based) about the centre coordinate by some angle theta (degrees) with matrices
*/
int rotatePolygon2DFM(POLYGON2DF_PTR polygon, int thetaDeg);

/*
* Scales the polygon (assumed int based) by sx > 0 and sy > 0 with matrices
*/
int scalePolygon2DIM(POLYGON2DI_PTR polygon, float sx, float sy);

/*
* Scales the polygon (assumed float based) by sx > 0 and sy > 0 with matrices
*/
int scalePolygon2DFM(POLYGON2DF_PTR polygon, float sx, float sy);