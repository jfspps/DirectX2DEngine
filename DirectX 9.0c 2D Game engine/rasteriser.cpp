#include "main.h"

extern int minClipX;
extern int maxClipX;
extern int minClipY;
extern int maxClipY;

extern float cosLookup[360];
extern float sinLookup[360];

int drawLine8bit(int x0, int y0, int x1, int y1, 
				 UCHAR colour, UCHAR *videoBuffer, int lpitch){

	int dx,   // difference in x's
		dy,   // difference in y's
		dx2,  // equals (dx*2), used to calc. error
		dy2,  // equals (dy*2), used to calc. error
		xInc, // no. of pixels to move along x-axis
	    yInc, // no. of pixels to move along y-axis
		decision, // decision variable
		index; // used for looping

	// pre-compute first pixel address in video buffer
	videoBuffer = videoBuffer + x0 + y0*lpitch;

	// compute horizontal and vertical deltas
	dx = x1 - x0;
	dy = y1 - y0;

	// test which direction the line is going in 
	// and then expect x to change by one unit
	if (dx >= 0){
		xInc = 1;
	} else if (dx < 0) {
		xInc = -1;
		dx = -dx; // need absolute value
	} else {
		dx = 0;
	}

	// test which direction the line is going in 
	// and then expect y to change by one scan line (pitch)
	if (dy >= 0){
		yInc = lpitch;
	} else if (dy < 0) {
		yInc = -lpitch;
		dy = -dy;  // need absolute value
	} else {
		dy = 0;
	}

	// prep
	dx2 = dx << 1;
	dy2 = dy << 1;


	if (dx > dy){
		// |m| < 1, eventually we will 
		// move one unit along the x-axis;
		// ignore the else block, this will never run
		// in this case

		// initialise, then update decision in y
		decision = dy2 - dx;
		
		// draw the line
		for (index = 0; index <= dx; index++){
			// plot pixel
			*videoBuffer = colour;
			
			if (decision >= 0){
				// ready to move pointer to next row (y);
				// bring back decision and test again later
				decision -= dx2;
				videoBuffer += yInc;
			}
			
			decision += dy2;
			
			// move to the next column (x)
			videoBuffer += xInc;
		}
	} else {
		// |m| >= 1, eventually we will 
		// move one unit along the y-axis;
		// ignore the if block, that will never run
		// in this case
		decision = dx2 - dy;
		
		// draw the line
		for (index = 0; index <= dy; index++){
			// set the pixel
			*videoBuffer = colour;
			
			// test if error overflowed
			if (decision >= 0){
				// ready to move pointer to next column (x);
				// bring back decision and test again later
				decision -= dy2;
				videoBuffer += xInc;
			}
			
			decision += dx2;
			
			// move to the next row (y)
			videoBuffer += yInc;
		}
	}

	// return success
	return(1);
}

int drawLine16bit(int x0, int y0, int x1, int y1, 
				  USHORT colour, UCHAR *videoBuffer, int lpitch){

	int dx,   // difference in x's
		dy,   // difference in y's
		dx2,  // equals (dx*2), used to calc. error
		dy2,  // equals (dy*2), used to calc. error
		xInc, // no. of pixels to move along x-axis
	    yInc, // no. of pixels to move along y-axis
		decision, // decision variable
		index; // used for looping

	int lpitch2 = lpitch >> 1;

	// pre-compute first pixel address in video buffer based on 16bit data
	USHORT *videoBufferUShort = (USHORT *)videoBuffer + x0 + y0*lpitch2;

	// compute horizontal and vertical deltas
	dx = x1 - x0;
	dy = y1 - y0;

	// test which direction the line is going in 
	// and then expect x to change by one unit
	if (dx >= 0){
		xInc = 1;
	} else if (dx < 0) {
		xInc = -1;
		dx = -dx; // need absolute value
	} else {
		dx = 0;
	}

	// test which direction the line is going in 
	// and then expect y to change by one scan line (pitch)
	if (dy >= 0){
		yInc = lpitch2;
	} else if (dy < 0) {
		yInc = -lpitch2;
		dy = -dy;  // need absolute value
	} else {
		dy = 0;
	}

	// prep
	dx2 = dx << 1;
	dy2 = dy << 1;


	if (dx > dy){
		// |m| < 1, eventually we will 
		// move one unit along the x-axis;
		// ignore the else block, this will never run
		// in this case

		// initialise, then update decision in y
		decision = dy2 - dx;
		
		// draw the line
		for (index = 0; index <= dx; index++){
			// plot pixel
			*videoBufferUShort = (USHORT)colour;
			
			if (decision >= 0){
				// ready to move pointer to next row (y);
				// bring back decision and test again later
				decision -= dx2;
				videoBufferUShort += yInc;
			}
			
			decision += dy2;
			
			// move to the next column (x)
			videoBufferUShort += xInc;
		}
	} else {
		// |m| >= 1, eventually we will 
		// move one unit along the y-axis;
		// ignore the if block, that will never run
		// in this case
		decision = dx2 - dy;
		
		// draw the line
		for (index = 0; index <= dy; index++){
			// set the pixel
			*videoBufferUShort = (USHORT)colour;
			
			// test if error overflowed
			if (decision >= 0){
				// ready to move pointer to next column (x);
				// bring back decision and test again later
				decision -= dy2;
				videoBufferUShort += xInc;
			}
			
			decision += dx2;
			
			// move to the next row (y)
			videoBufferUShort += yInc;
		}
	}

	// return success
	return(1);
}

int clipLine(int &x1, int &y1, int &x2, int &y2){

	// internal clipping codes
	#define CLIP_CODE_C  0x0000 // completely in clipping region
	#define CLIP_CODE_N  0x0008 // crosses top edge (bits 1000)
	#define CLIP_CODE_S  0x0004 // crosses bottom edge (bits 0100)
	#define CLIP_CODE_E  0x0002 // crosses right edge (bits 0010)
	#define CLIP_CODE_W  0x0001 // crosses left edge (bits 0001)

	#define CLIP_CODE_NE 0x000a // bits 1010
	#define CLIP_CODE_SE 0x0006 // bits 0110
	#define CLIP_CODE_NW 0x0009 // bits 1001
	#define CLIP_CODE_SW 0x0005 // bits 0101

	// this function updates the passed coords;
	// store them here temporarily and update
	// at the end if all OK
	int x1Clipped = x1, 
		y1Clipped = y1, 
		x2Clipped = x2, 
		y2Clipped = y2;

	// points 1 and 2 clip codes
	int p1_code = 0, 
		p2_code = 0;

	// determine codes for p1 and p2
	if (y1 < minClipY)
		p1_code |= CLIP_CODE_S;
	else
	if (y1 > maxClipY)
		p1_code |= CLIP_CODE_N;

	if (x1 < minClipX)
		p1_code |= CLIP_CODE_W;
	else
	if (x1 > maxClipX)
		p1_code |= CLIP_CODE_E;

	if (y2 < minClipY)
		p2_code |= CLIP_CODE_S;
	else
	if (y2 > maxClipY)
		p2_code |= CLIP_CODE_N;

	if (x2 < minClipX)
		p2_code |= CLIP_CODE_W;
	else
	if (x2 > maxClipX)
		p2_code |= CLIP_CODE_E;

	// try and trivially reject
	if ((p1_code & p2_code)) 
		return(0);

	// test for totally visible, if so leave points untouched
	if (p1_code == 0 && p2_code == 0)
		return(1);

	// determine end clip point for p1
	switch(p1_code){
		case CLIP_CODE_C: 
			break;

		case CLIP_CODE_N:
			{
				y1Clipped = maxClipY;
				x1Clipped = x1 + 0.5+(maxClipY-y1)*(x2-x1)/(y2-y1);
			} break;

		case CLIP_CODE_S:
			{
				y1Clipped = minClipY;
				x1Clipped = x1 + 0.5+(minClipY-y1)*(x2-x1)/(y2-y1);
			} break;

		case CLIP_CODE_W:
			{
				x1Clipped = minClipX;
				y1Clipped = y1 + 0.5+(minClipX-x1)*(y2-y1)/(x2-x1);
			} break;

		case CLIP_CODE_E:
			{
				x1Clipped = maxClipX;
				y1Clipped = y1 + 0.5+(maxClipX-x1)*(y2-y1)/(x2-x1);
			} break;

		// these cases are more complex, must compute 2 intersections
		case CLIP_CODE_NE:
			{
				// north hline intersection
				y1Clipped = maxClipY;
				x1Clipped = x1 + 0.5+(maxClipY-y1)*(x2-x1)/(y2-y1);

				// test if intersection is valid, of so then done, else compute next
				if (x1Clipped < minClipX || x1Clipped > maxClipX){
					// east vline intersection
					x1Clipped = maxClipX;
					y1Clipped = y1 + 0.5+(maxClipX-x1)*(y2-y1)/(x2-x1);
				}
			} break;

		case CLIP_CODE_SE:
			{
				// south hline intersection
				y1Clipped = minClipY;
				x1Clipped = x1 + 0.5+(minClipY-y1)*(x2-x1)/(y2-y1);
				
				// test if intersection is valid, of so then done, else compute next
				if (x1Clipped < minClipX || x1Clipped > maxClipX){
					// east vline intersection
					x1Clipped = maxClipX;
					y1Clipped = y1 + 0.5+(maxClipX-x1)*(y2-y1)/(x2-x1);
				}
			} break;
		 
		case CLIP_CODE_NW:
			{
				// north hline intersection
				y1Clipped = maxClipY;
				x1Clipped = x1 + 0.5+(maxClipY-y1)*(x2-x1)/(y2-y1);
				
				// test if intersection is valid, of so then done, else compute next
				if (x1Clipped < minClipX || x1Clipped > maxClipX){
					x1Clipped = minClipX;
					y1Clipped = y1 + 0.5+(minClipX-x1)*(y2-y1)/(x2-x1);	
				}
			} break;
		
		case CLIP_CODE_SW:
			{
				// south hline intersection
				y1Clipped = minClipY;
				x1Clipped = x1 + 0.5+(minClipY-y1)*(x2-x1)/(y2-y1);
				
				// test if intersection is valid, of so then done, else compute next
				if (x1Clipped < minClipX || x1Clipped > maxClipX){
					x1Clipped = minClipX;
					y1Clipped = y1 + 0.5+(minClipX-x1)*(y2-y1)/(x2-x1);
				}
			} break;
		
		default:break;
	}

	// determine clip point for p2
	switch(p2_code){
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
			{
				y2Clipped = maxClipY;
				x2Clipped = x2 + (maxClipY-y2)*(x1-x2)/(y1-y2);
			} break;
		
		case CLIP_CODE_S:
			{
				y2Clipped = minClipY;
				x2Clipped = x2 + (minClipY-y2)*(x1-x2)/(y1-y2);
			} break;
		
		case CLIP_CODE_W:
			{
				x2Clipped = minClipX;
				y2Clipped = y2 + (minClipX-x2)*(y1-y2)/(x1-x2);
			} break;
		
		case CLIP_CODE_E:
			{
				x2Clipped = maxClipX;
				y2Clipped = y2 + (maxClipX-x2)*(y1-y2)/(x1-x2);
			} break;
			
		// these cases are more complex, must compute 2 intersections
		case CLIP_CODE_NE:
			{
				// north hline intersection
				y2Clipped = maxClipY;
				x2Clipped = x2 + 0.5 + (maxClipY-y2)*(x1-x2)/(y1-y2);
				
				// test if intersection is valid, of so then done, else compute next
				if (x2Clipped < minClipX || x2Clipped > maxClipX){
					// east vline intersection
					x2Clipped = maxClipX;
					y2Clipped = y2 + 0.5 + (maxClipX-x2)*(y1-y2)/(x1-x2);
				}
			} break;
		
		case CLIP_CODE_SE:
			{
				// south hline intersection
				y2Clipped = minClipY;
				x2Clipped = x2 + 0.5 + (minClipY-y2)*(x1-x2)/(y1-y2);
				
				// test if intersection is valid, of so then done, else compute next
				
				if (x2Clipped < minClipX || x2Clipped > maxClipX){
					// east vline intersection
					x2Clipped = maxClipX;
					y2Clipped = y2 + 0.5 + (maxClipX-x2)*(y1-y2)/(x1-x2);
				}
			} break;
		
		case CLIP_CODE_NW:
			{
				// north hline intersection
				y2Clipped = maxClipY;
				x2Clipped = x2 + 0.5 + (maxClipY-y2)*(x1-x2)/(y1-y2);
				
				// test if intersection is valid, of so then done, else compute next
				if (x2Clipped < minClipX || x2Clipped > maxClipX){
					x2Clipped = minClipX;
					y2Clipped = y2 + 0.5 + (minClipX-x2)*(y1-y2)/(x1-x2);
				}
			} break;
		
		case CLIP_CODE_SW:
			{
				// south hline intersection
				y2Clipped = minClipY;
				x2Clipped = x2 + 0.5 + (minClipY-y2)*(x1-x2)/(y1-y2);
				
				// test if intersection is valid, of so then done, else compute next
				if (x2Clipped < minClipX || x2Clipped > maxClipX)
				{
					x2Clipped = minClipX;
					y2Clipped = y2 + 0.5 + (minClipX-x2)*(y1-y2)/(x1-x2);
				}
			} break;
		
		default:break;
	}

	// do bounds check
	if ((x1Clipped < minClipX) || (x1Clipped > maxClipX) ||
		(y1Clipped < minClipY) || (y1Clipped > maxClipY) ||
		(x2Clipped < minClipX) || (x2Clipped > maxClipX) ||
		(y2Clipped < minClipY) || (y2Clipped > maxClipY) ){
			return(0);
	} 

	// update coords
	x1 = x1Clipped;
	y1 = y1Clipped;
	x2 = x2Clipped;
	y2 = y2Clipped;

	return(1);
}

int drawClipLine8bit(int x0, int y0, int x1, int y1, UCHAR color, 
					 UCHAR *videoBuffer, int lpitch){

	int cxs, cys,
		cxe, cye;

	// clip and draw each line
	cxs = x0;
	cys = y0;
	cxe = x1;
	cye = y1;

	// clip the line
	if (clipLine(cxs, cys, cxe, cye))
		drawLine8bit(cxs, cys, cxe, cye, color, videoBuffer, lpitch);

	// return success
	return(1);
}

int draw2DIPolygon8Bit(POLYGON2DI_PTR polygon, UCHAR *videoBuffer, int lPitch){
	if (polygon->state){
		int index;
		for (index = 0; index < polygon->numVerts - 1; index++){
			drawClipLine8bit(
				// first endpoint
				polygon->vList[index].x + polygon->x0,
				polygon->vList[index].y + polygon->y0,
				// next endpoint
				polygon->vList[index + 1].x + polygon->x0,
				polygon->vList[index + 1].y + polygon->y0,
				polygon->colour,
				videoBuffer,
				lPitch);
		}

		// close the polygon
		drawClipLine8bit(
				// first endpoint
				polygon->vList[0].x + polygon->x0,
				polygon->vList[0].y + polygon->y0,
				// next endpoint
				polygon->vList[index].x + polygon->x0,
				polygon->vList[index].y + polygon->y0,
				polygon->colour,
				videoBuffer,
				lPitch);

		return(1);
	}

	return(0);
}

int draw2DFPolygon8Bit(POLYGON2DF_PTR polygon, UCHAR *videoBuffer, int lPitch){
	if (polygon->state){
		int index;
		for (index = 0; index < polygon->numVerts - 1; index++){
			drawClipLine8bit(
				// first endpoint
				polygon->vList[index].x + polygon->x0,
				polygon->vList[index].y + polygon->y0,
				// next endpoint
				polygon->vList[index + 1].x + polygon->x0,
				polygon->vList[index + 1].y + polygon->y0,
				polygon->colour,
				videoBuffer,
				lPitch);
		}

		// close the polygon
		drawClipLine8bit(
				// first endpoint
				polygon->vList[0].x + polygon->x0,
				polygon->vList[0].y + polygon->y0,
				// next endpoint
				polygon->vList[index].x + polygon->x0,
				polygon->vList[index].y + polygon->y0,
				polygon->colour,
				videoBuffer,
				lPitch);

		return(1);
	}

	return(0);
}

int translatePolygon2DI(POLYGON2DI_PTR polygon, int dx, int dy){
	if (!polygon)
	   return(0);

	// translate
	polygon->x0 += dx;
	polygon->y0 += dy;

	// return success
	return(1);
}

int translatePolygon2DF(POLYGON2DF_PTR polygon, int dx, int dy){
	if (!polygon)
	   return(0);

	// translate
	polygon->x0 += dx;
	polygon->y0 += dy;

	// return success
	return(1);
}

int rotatePolygon2D(POLYGON2DF_PTR polygon, int thetaDeg){
	if (!polygon)
		return(0);

	for (int currVert = 0; currVert < polygon->numVerts; currVert++){
		
		float xr = (float) polygon->vList[currVert].x * cosLookup[thetaDeg] - 
						(float) polygon->vList[currVert].y * sinLookup[thetaDeg];

		float yr = (float)polygon->vList[currVert].x * sinLookup[thetaDeg] + 
						(float)polygon->vList[currVert].y * cosLookup[thetaDeg];

		polygon->vList[currVert].x = xr;
		polygon->vList[currVert].y = yr;
	}

	// return success
	return(1);
}

int scalePolygon2DF(POLYGON2DF_PTR polygon, float sx, float sy){
	// validate
	if (!polygon || sx <= 0 || sy <= 0)
	   return(0);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		// scale and store result back
		polygon->vList[curr_vert].x *= sx;
		polygon->vList[curr_vert].y *= sy;
	}

	// return success
	return(1);
}

int scalePolygon2DI(POLYGON2DI_PTR polygon, float sx, float sy){
	// validate
	if (!polygon || sx <= 0 || sy <= 0)
	   return(0);

	// loop and scale each point
	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		// scale and store result back
		polygon->vList[curr_vert].x *= sx;
		polygon->vList[curr_vert].y *= sy;
	}

	// return success
	return(1);
}

int matrixInit3X3(MATRIX3X3_PTR ma,
				  float m00, float m01, float m02,
				  float m10, float m11, float m12,
				  float m20, float m21, float m22){

	ma->M[0][0] = m00; ma->M[0][1] = m01; ma->M[0][2] = m02; 
	ma->M[1][0] = m10; ma->M[1][1] = m11; ma->M[1][2] = m12; 
	ma->M[2][0] = m20; ma->M[2][1] = m21; ma->M[2][2] = m22; 

	// return success
	return(1);
}

int matrixMultiply1X3_3X3(MATRIX1X3_PTR ma, MATRIX3X3_PTR mb, MATRIX1X3_PTR mprod){
    for (int col = 0; col < 3; col++){
        // compute dot product from row of ma 
        // and column of mb

        float sum = 0;

        for (int row = 0; row < 3; row++){
			// add in next product pair
			sum += (ma->M[row] * mb->M[row][col]);
		}

        mprod->M[col] = sum;
	}
	
	return(1);
}

int translatePolygon2DIM(POLYGON2DI_PTR polygon, int dx, int dy){
	if (!polygon)
	   return(0);

	MATRIX3X3 mt; // used to hold translation transform matrix

	// initialize the translation matrix
	matrixInit3X3(&mt,1,0,0, 0,1,0, dx,dy,1); 

	// create a 1x3 matrix to do the transform
	MATRIX1X3 p0 = {polygon->x0, polygon->y0, 0};
	MATRIX1X3 pTemp = {0,0,0};

	// now translate via a matrix multiply
	matrixMultiply1X3_3X3(&p0, &mt, &pTemp);

	// now copy the result back into polygon
	polygon->x0 = pTemp.M[0];
	polygon->y0 = pTemp.M[1];

	// return success
	return(1);
}

int translatePolygon2DFM(POLYGON2DF_PTR polygon, int dx, int dy){
	if (!polygon)
	   return(0);

	MATRIX3X3 mt; // used to hold translation transform matrix

	// initialize the translation matrix
	matrixInit3X3(&mt,1,0,0, 0,1,0, dx,dy,1); 

	// create a 1x3 matrix to do the transform
	MATRIX1X3 p0 = {polygon->x0, polygon->y0, 0};
	MATRIX1X3 pTemp = {0,0,0};

	// now translate via a matrix multiply
	matrixMultiply1X3_3X3(&p0, &mt, &pTemp);

	// now copy the result back into polygon
	polygon->x0 = pTemp.M[0];
	polygon->y0 = pTemp.M[1];

	// return success
	return(1);
}

int rotatePolygon2DIM(POLYGON2DI_PTR polygon, int thetaDeg){
	if (!polygon)
	   return(0);

	// test for negative rotation angle
	if (thetaDeg < 0)
	   thetaDeg += 360;

	MATRIX3X3 mr; // used to hold rotation transform matrix

	matrixInit3X3(&mr, cosLookup[thetaDeg],sinLookup[thetaDeg],0,
					 -sinLookup[thetaDeg],cosLookup[thetaDeg],0, 
					  0,0,1); 

	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		MATRIX1X3 p0 = {polygon->vList[curr_vert].x, polygon->vList[curr_vert].y, 0};
		MATRIX1X3 pTemp;

		// now rotate via the rotation matrix mr
		matrixMultiply1X3_3X3(&p0, &mr, &pTemp);

		// now copy the result back into vertex
		polygon->vList[curr_vert].x = pTemp.M[0];
		polygon->vList[curr_vert].y = pTemp.M[1];
	}

	// return success
	return(1);
}

int rotatePolygon2DFM(POLYGON2DF_PTR polygon, int thetaDeg){
	if (!polygon)
	   return(0);

	// test for negative rotation angle
	if (thetaDeg < 0)
	   thetaDeg += 360;

	MATRIX3X3 mr; // used to hold rotation transform matrix

	matrixInit3X3(&mr, cosLookup[thetaDeg],sinLookup[thetaDeg],0,
					 -sinLookup[thetaDeg],cosLookup[thetaDeg],0, 
					  0,0,1); 

	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		MATRIX1X3 p0 = {polygon->vList[curr_vert].x, polygon->vList[curr_vert].y, 0};
		MATRIX1X3 pTemp;

		// now rotate via the rotation matrix mr
		matrixMultiply1X3_3X3(&p0, &mr, &pTemp);

		// now copy the result back into vertex
		polygon->vList[curr_vert].x = pTemp.M[0];
		polygon->vList[curr_vert].y = pTemp.M[1];
	}

	// return success
	return(1);
}

int scalePolygon2DIM(POLYGON2DI_PTR polygon, float sx, float sy){
	if (!polygon || sx <=0 || sy <= 0)
	   return(0);

	MATRIX3X3 ms; // used to hold scaling transform matrix

	matrixInit3X3(&ms,sx,0,0, 
					  0,sy,0, 
					  0,0,1); 

	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		MATRIX1X3 p0 = {polygon->vList[curr_vert].x, polygon->vList[curr_vert].y, 0};
		MATRIX1X3 pTemp = {0,0,0};

		// now scale via a matrix multiply
		matrixMultiply1X3_3X3(&p0, &ms, &pTemp);

		// now copy the result back into vertex
		polygon->vList[curr_vert].x = pTemp.M[0];
		polygon->vList[curr_vert].y = pTemp.M[1];
	}

	// return success
	return(1);
}

int scalePolygon2DFM(POLYGON2DF_PTR polygon, float sx, float sy){
	if (!polygon || sx <=0 || sy <= 0)
	   return(0);

	MATRIX3X3 ms; // used to hold scaling transform matrix

	matrixInit3X3(&ms,sx,0,0, 
					  0,sy,0, 
					  0,0,1); 

	for (int curr_vert = 0; curr_vert < polygon->numVerts; curr_vert++){
		MATRIX1X3 p0 = {polygon->vList[curr_vert].x, polygon->vList[curr_vert].y, 0};
		MATRIX1X3 pTemp = {0,0,0};

		// now scale via a matrix multiply
		matrixMultiply1X3_3X3(&p0, &ms, &pTemp);

		// now copy the result back into vertex
		polygon->vList[curr_vert].x = pTemp.M[0];
		polygon->vList[curr_vert].y = pTemp.M[1];
	}

	// return success
	return(1);
}

void drawFlatTopTriangle(int x1, int y1,
						 int x2, int y2,
						 int x3, int y3,
						 int colour, 
						 UCHAR *dest_buffer, int mempitch){

	float dx_right,    // the dx/dy ratio of the right edge of line
		  dx_left,     // the dx/dy ratio of the left edge of line
		  xStart, xEnd,    
		  height;

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	// destination address of next scanline
	UCHAR *dest_addr = NULL;

	// test order of x1 and x2
	if (x2 < x1){
		temp_x = x2;
		x2 = x1;
		x1 = temp_x;
	}

	// compute delta's
	height = y3 - y1;

	dx_left  = (x3 - x1) / height;
	dx_right = (x3 - x2) / height;

	// set starting points
	xStart = (float) x1;
	xEnd = (float) x2 + (float) 0.5;

	// perform y clipping
	if (y1 < minClipY){
	   xStart += dx_left*(float)(-y1 + minClipY);
	   xEnd += dx_right*(float)(-y1 + minClipY);

	   // reset y1
	   y1 = minClipY;
	}

	if (y3 > maxClipY)
	   y3 = maxClipY;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= minClipX && x1 <= maxClipX &&
		x2 >= minClipX && x2 <= maxClipX &&
		x3 >= minClipX && x3 <= maxClipX){
			
			for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch){
				memset((UCHAR *)dest_addr + (unsigned int)xStart,
					colour, (unsigned int)(xEnd - xStart + 1));

			// adjust starting point and ending point
			xStart += dx_left;
			xEnd += dx_right;
			}
		} else {
			// clip x axis with slower version
			
			for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch){
				// do x clip
				left  = (int)xStart;
				right = (int)xEnd;
				
				// adjust starting point and ending point
				xStart += dx_left;
				xEnd += dx_right;
				
				// clip line
				if (left < minClipX){
					left = minClipX;

					if (right < minClipX)
						continue;
				}

				if (right > maxClipX){
					right = maxClipX;
					
					if (left > maxClipX)
						continue;
				}
				
				memset((UCHAR *) dest_addr + (unsigned int)left,
					colour, (unsigned int)(right-left+1));
			}
	}
}

void drawFlatBottomTriangle(int x1, int y1,
							int x2, int y2,
							int x3, int y3,
							int colour,
							UCHAR *dest_buffer, int mempitch){

	float dx_right,    // the dx/dy ratio of the right edge of line
		dx_left,     // the dx/dy ratio of the left edge of line
		xStart, xEnd,
		height;

	int temp_x,        // used during sorting as temps
		temp_y,
		right,         // used by clipping
		left;

	// destination address of next scanline
	UCHAR *dest_addr;

	// test order of x1 and x2
	if (x3 < x2){
		temp_x = x2;
		x2     = x3;
		x3     = temp_x;
	}

	// compute delta's
	height = y3 - y1;

	dx_left  = (x2 - x1) / height;
	dx_right = (x3 - x1) / height;

	// set starting points
	xStart = (float) x1;
	xEnd = (float) x1; // +(float)0.5;

	// perform y clipping
	if (y1 < minClipY){
		xStart += dx_left * (float)(-y1 + minClipY);
		xEnd += dx_right * (float)(-y1 + minClipY);

		// reset y1
		y1 = minClipY;
	}

	if (y3 > maxClipY)
		y3 = maxClipY;

	// compute starting address in video memory
	dest_addr = dest_buffer + y1*mempitch;

	// test if x clipping is needed
	if (x1 >= minClipX && x1 <= maxClipX &&
		x2 >= minClipX && x2 <= maxClipX &&
		x3 >= minClipX && x3 <= maxClipX){
			
			for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch){
				memset((UCHAR  *)dest_addr + (unsigned int)xStart,
					colour,(unsigned int)(xEnd - xStart + 1));

			// adjust starting point and ending point
			xStart += dx_left;
			xEnd += dx_right;
		}
	} else {
		// clip x axis with slower version

		for (temp_y = y1; temp_y <= y3; temp_y++, dest_addr += mempitch){
			// do x clip
			left  = (int)xStart;
			right = (int)xEnd;

			// adjust starting point and ending point
			xStart += dx_left;
			xEnd += dx_right;

			// clip line
			if (left < minClipX){
				left = minClipX;

				if (right < minClipX)
					continue;
			}

			if (right > maxClipX){
				right = maxClipX;

				if (left > maxClipX)
					continue;
			}
			
			memset((UCHAR *)dest_addr + (unsigned int)left,
				colour,(unsigned int)(right - left + 1));
		}
	}
}

void drawTriangle2DI(int x1, int y1,
					  int x2, int y2,
					  int x3, int y3,
					  int colour,
					  UCHAR *dest_buffer, int mempitch){

	int temp_x, // used for sorting
		temp_y,
		new_x;

	// test for h lines and v lines
	if ((x1 == x2 && x2 == x3)  ||  (y1 == y2 && y2 == y3))
		return;

	// sort p1, p2, p3 in ascending y order
	if (y2 < y1){
		temp_x = x2;
		temp_y = y2;
		x2     = x1;
		y2     = y1;
		x1     = temp_x;
		y1     = temp_y;
	}

	// now we know that p1 and p2 are in order
	if (y3 < y1){
		temp_x = x3;
		temp_y = y3;
		x3     = x1;
		y3     = y1;
		x1     = temp_x;
		y1     = temp_y;
	}

	// finally test y3 against y2
	if (y3 < y2){
		temp_x = x3;
		temp_y = y3;
		x3     = x2;
		y3     = y2;
		x2     = temp_x;
		y2     = temp_y;

	}

	// do trivial rejection tests for clipping
	if ( y3 < minClipY || y1 > maxClipY ||
		(x1 < minClipX && x2 < minClipX && x3 < minClipX) ||
		(x1 > maxClipX && x2 > maxClipX && x3 > maxClipX) )
		return;

	// test if top of triangle is flat
	if (y1 == y2){
		drawFlatTopTriangle(x1, y1, x2, y2, x3, y3, colour, dest_buffer, mempitch);
	} else if (y2 == y3){
		drawFlatBottomTriangle(x1, y1, x2, y2, x3, y3, colour, dest_buffer, mempitch);
	} else {
		// general triangle that's needs to be broken up along long edge
		new_x = x1 + (int)(0.5+(float)(y2 - y1) * (float)(x3 - x1)/(float)(y3 - y1));

		// draw each sub-triangle
		drawFlatBottomTriangle(x1, y1, new_x, y2, x2, y2, colour, dest_buffer, mempitch);
		drawFlatTopTriangle(x2, y2, new_x, y2, x3, y3, colour, dest_buffer, mempitch);
	}
}
