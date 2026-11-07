#pragma once

int drawLine8bit(
  int x0, int y0, int x1, int y1, UCHAR colour, UCHAR *videoBuffer, int lpitch);

int drawLine16bit(
  int x0, int y0, int x1, int y1, USHORT colour, UCHAR *videoBuffer, int lpitch);