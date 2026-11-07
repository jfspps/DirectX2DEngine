#include "main.h"

// two endpoints at (x0, y0) and (x2, y1)
int drawLine8bit(
  int x0, int y0, int x1, int y1, UCHAR colour, UCHAR *videoBuffer, int lpitch) {

  int x, y, 
	dx,         // difference in x's
    dy,         // difference in y's
    dx2,        // equals (dx*2), used to calc. decX
    dy2,        // equals (dy*2), used to calc. decY
    xInc,       // no. of pixels to move along x-axis
    yInc,       // no. of pixels to move along y-axis
    decX,       // the decision variable in x
    decY;       // the decision variable in y

  // pre-compute first pixel address in video buffer
  videoBuffer = videoBuffer + x0 + y0*lpitch;

  dx = x1 - x0;
  dy = y1 - y0;

  // test which direction the line is going in 
  // and then expect x to change by one unit
  if (dx >= 0){
    xInc = 1;
  } else if (dx < 0) {
    xInc = -1;

    dx = -dx;  // need |dx|
  } else {
    xInc = 0;
  }

  // test which direction the line is going in 
  // and then expect y to change by one scan line (pitch)
  if (dy >= 0){
    yInc = lpitch;
  } else if (dy < 0){
    yInc = -lpitch;

    dy = -dy;  // need |dy|
  } else {
    yInc = 0;
  }

  // constants required for the decision terms
  dx2 = dx << 1;
  dy2 = dy << 1;

  // now draw the line based on the gradient
  if (dx > dy){
    // |m| < 1, eventually we will 
    // move one unit along the x-axis;
    // ignore the else block, this will never run
    // in this case

    // initialise, then update decision in y
    for (decY = dy2 - dx; /**/; x += xInc, decY += dy2){
      // plot pixel
      *videoBuffer = colour;

      if (x == x1){
        // at an endpoint, leave
        break;
      }

      if (decY >= 0){
        // ready to move pointer to next row (y);
        // bring back decY and test again later
        decY -= dx2;

        videoBuffer += yInc;
      }
    }

  } else {
    // |m| >= 1, eventually we will 
    // move one unit along the y-axis;
    // ignore the if block, that will never run
    // in this case

    // initialise, then update decision in x
    for (decX = dx2 - dy; /**/; y += yInc, decX += dx2){
      // plot pixel
      *videoBuffer = colour;

      if (y == y1){
        // at an endpoint, leave
        break;
      }

      if (decX >= 0){
        // ready to move pointer to next column (x);
        // bring back decX and test again later
        decX -= dy2;

        videoBuffer += xInc;
      }
    }
  } 

  return(1);
}

// two endpoints at (x0, y0) and (x2, y1)
int drawLine16bit(
  int x0, int y0, int x1, int y1, USHORT colour, UCHAR *videoBuffer, int lpitch) {

  int x, y,
	dx,         // difference in x's
    dy,         // difference in y's
    dx2,        // equals (dx*2), used to calc. decX
    dy2,        // equals (dy*2), used to calc. decY
    xInc,       // no. of pixels to move along x-axis
    yInc,       // no. of pixels to move along y-axis
    decX,       // the decision variable in x
    decY;       // the decision variable in y

  int lpitch2 = lpitch >> 1; // USHORT strided lpitch

  // pre-compute first pixel address in video buffer based on 16bit data
  USHORT *videoBufferUShort = (USHORT *)videoBuffer + x0 + y0*lpitch2;

  dx = x1 - x0;
  dy = y1 - y0;

  // test which direction the line is going in 
  // and then expect x to change by one unit
  if (dx >= 0){
    xInc = 1;
  } else if (dx < 0){
    xInc = -1;

    dx = -dx;  // need |dx|
  } else {
    xInc = 0;
  }

  // test which direction the line is going in 
  // and then expect y to change by one scan line (pitch)
  if (dy >= 0){
    yInc = lpitch2;
  } else if (dy < 0) {
    yInc = -lpitch2;

    dy = -dy;  // need |dy|
  } else {
    yInc = 0;
  }

  // constants required for the decision terms
  dx2 = dx << 1;
  dy2 = dy << 1;

  // now draw the line based on the gradient
  if (dx > dy){
    // |m| < 1, eventually we will 
    // move one unit along the x-axis;
    // ignore the else block, this will never run
    // in this case

    // initialise, then update decision in y
    for (decY = dy2 - dx; /**/; x += xInc, decY += dy2){
      // plot pixel
      *videoBufferUShort = (USHORT)colour;

      if (x == x1){
        // at an endpoint, leave
        break;
      }

      if (decY >= 0){
        // ready to move pointer to next row (y);
        // bring back decY and test again later
        decY -= dx2;

        videoBufferUShort += yInc;
      }
    }

  } else {
    // |m| >= 1, eventually we will 
    // move one unit along the y-axis;
    // ignore the if block, that will never run
    // in this case

    // initialise, then update decision in x
    for (decX = dx2 - dy; /**/; y += yInc, decX += dx2){
      // plot pixel
      *videoBufferUShort = (USHORT)colour;

      if (y == y1){
        // at an endpoint, leave
        break;
      }

      if (decX >= 0){
        // ready to move pointer to next column (x);
        // bring back decX and test again later
        decX -= dy2;

        videoBufferUShort += xInc;
      }
    }
  } 

  return(1);
}
