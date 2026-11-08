#include "main.h"

extern int minClipX;
extern int maxClipX;
extern int minClipY;
extern int maxClipY;

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
