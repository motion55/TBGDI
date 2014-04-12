
/* ********************************************************************** */
/*                                                                        */
/*   LINE.C                                                               */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "line.h"

/* -- PRIVATE FUNCTION DECLARATIONS ------------------------------------- */

static bool ClipLine(short *const xa, short *const ya,
	short *const xb, short *const yb);
static unsigned char GetOutcode(short x, short y);
static void DrawSolidLine(unsigned short xa, unsigned short ya,
	unsigned short xb, unsigned short yb);
static void DrawDashedLine(unsigned short xa, unsigned short ya,
	unsigned short xb, unsigned short yb);
static void DrawWideLine(short xa, short ya, short xb, short yb,
	short thickness, unsigned char fillType);
static void DrawGridDots32(long spacing, long divisor);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineAbsolute                                                 */
/*                                                                        */
/* ====================================================================== */

void DrawWideLineAbsolute(short xa, short ya, short xb, short yb)
{
	short dx, dy;

	DrawingPositionX = xa;
	dx = (xb - xa);
	DrawingPositionY = ya;
	dy = (yb - ya);

	DrawWideLineRelative(dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineRelative                                                 */
/*                                                                        */
/* ====================================================================== */

void DrawWideLineRelative(short dx, short dy)
{
	unsigned short savedX;
	unsigned short savedY;
	unsigned short xa;
	unsigned short ya;
	unsigned short xb;
	unsigned short yb;
	unsigned short index;

	static short table[64] = {
		 0, -1,  0, -1,  0,  1,  0,  1,
		-1,  0, -1,  0,  1,  0,  1,  0,
		-1,  0,  0, -1,  0,  1,  1,  0,
		 0, -1,  1,  0, -1,  0,  0,  1,

		 0, -2,  0, -2,  0,  2,  0,  2,
		-2,  0, -2,  0,  2,  0,  2,  0,
		-2,  0,  0, -2,  0,  2,  2,  0,
		 0, -2,  2,  0, -2,  0,  0,  2
	};

	if (Scale > 2) {
		DrawSolidLineRelative(dx, dy);
		return;
	}

	if (dx == 0) {
		DrawingPositionX -= Scale;
		DrawSolidLineRelative(0, dy);
		DrawingPositionX += (Scale << 1);
		DrawSolidLineRelative(0, -dy);
		DrawingPositionX -= Scale;
		DrawSolidLineRelative(0, dy);
	} else if (dy == 0) {
		DrawingPositionY -= Scale;
		DrawSolidLineRelative(dx, 0);
		DrawingPositionY += (Scale << 1);
		DrawSolidLineRelative(-dx, 0);
		DrawingPositionY -= Scale;
		DrawSolidLineRelative(dx, 0);
	} else {
		if (abs(dx) < abs(dy)) {
			index = 0;
		} else if (abs(dx) > abs(dy)) {
			index = 8;
		} else if (dx == dy) {
			index = 24;
		} else {
			index = 16;
		}

		if (Scale == 2) {
			index += 32;
		}

		savedX = DrawingPositionX;
		savedY = DrawingPositionY;

		xa = savedX + table[index];
		ya = savedY + table[index+1];
		xb = savedX + dx + table[index+2];
		yb = savedY + dy + table[index+3];
		DrawLineFunction(xa, ya, xb, yb);

		xa = savedX + table[index+4];
		ya = savedY + table[index+5];
		xb = savedX + dx + table[index+6];
		yb = savedY + dy + table[index+7];
		DrawLineFunction(xa, ya, xb, yb);

		xa = savedX;
		ya = savedY;
		xb = savedX + dx;
		yb = savedY + dy;
		DrawLineFunction(xa, ya, xb, yb);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawDashedLineAbsolute                                               */
/*                                                                        */
/* ====================================================================== */

void DrawDashedLineAbsolute(short xa, short ya, short xb, short yb)
{
	DrawingPositionX = xb;
	DrawingPositionY = yb;

	if (ClipLine(&xa, &ya, &xb, &yb) == true) {
		DrawDashedLine(xa, ya, xb, yb);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawDashedLineRelative                                               */
/*                                                                        */
/* ====================================================================== */

void DrawDashedLineRelative(short dx, short dy)
{
	short xa;
	short ya;
	short xb;
	short yb;

	xa = DrawingPositionX;
	ya = DrawingPositionY;
	xb = xa + dx;
	yb = ya + dy;

	DrawDashedLineAbsolute(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawSolidLineAbsolute                                                */
/*                                                                        */
/* ====================================================================== */

void DrawSolidLineAbsolute(short xa, short ya, short xb, short yb)
{
	DrawingPositionX = xb;
	DrawingPositionY = yb;

	if (ClipLine(&xa, &ya, &xb, &yb) == true) {
		DrawSolidLine(xa, ya, xb, yb);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawSolidLineRelative                                                */
/*                                                                        */
/* ====================================================================== */

void DrawSolidLineRelative(short dx, short dy)
{
	short xa;
	short ya;
	short xb;
	short yb;

	xa = DrawingPositionX;
	ya = DrawingPositionY;
	xb = xa + dx;
	yb = ya + dy;

	DrawSolidLineAbsolute(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineAbsolute32                                               */
/*                                                                        */
/* ====================================================================== */

void DrawWideLineAbsolute32(long xa, long ya, long xb, long yb,
	long thickness, unsigned char fillType)
{
	unsigned char outcodeA;
	unsigned char outcodeB;

	long temp;
	long dx;
	long dy;

	long LeftEdge;
	long RightEdge;
	long TopEdge;
	long BottomEdge;

	if (fillType==255) {
		DrawGridDots32(thickness, yb);
	} else {
		LeftEdge	= WindowStartX32 - thickness;
		RightEdge	= WindowEndX32 + thickness;
		TopEdge		= WindowStartY32 - thickness;
		BottomEdge	= WindowEndY32 + thickness;

		outcodeB = INSIDE;
		if (yb>BottomEdge) {
			outcodeB = BOTTOM;
		} else
		if (yb<TopEdge) {
			outcodeB = TOP;
		}
		if (xb>RightEdge) {
			outcodeB |= RIGHT;
		} else
		if (xb<LeftEdge) {
			outcodeB |= LEFT;
		}

		dx = xb-xa;
		dy = yb-ya;

		for (;;) {
			outcodeA = INSIDE;
			if (ya>BottomEdge) {
				outcodeA = BOTTOM;
			} else
			if (ya<TopEdge) {
				outcodeA = TOP;
			}
			if (xa>RightEdge) {
				outcodeA |= RIGHT;
			} else
			if (xa<LeftEdge) {
				outcodeA |= LEFT;
			}

			if ((outcodeA==INSIDE)&&(outcodeB==INSIDE))
				break;

			if (outcodeA&outcodeB)
				return;

			if (outcodeA==INSIDE) {
				temp = xa; xa = xb; xb = temp;
				temp = ya; ya = yb; yb = temp;

				outcodeA = outcodeB;
				outcodeB = INSIDE;
			}

			if (outcodeA&BOTTOM) {
				xa += ((long long)(BottomEdge-ya)*(long long)dx)/dy;
				ya = BottomEdge;
			} else
			if (outcodeA&TOP) {
				xa += ((long long)(TopEdge-ya)*(long long)dx)/dy;
				ya = TopEdge;
			} else
			if (outcodeA&RIGHT) {
				ya += ((long long)(RightEdge-xa)*(long long)dy)/dx;
				xa = RightEdge;
			} else
			if (outcodeA&LEFT) {
				ya += ((long long)(LeftEdge-xa)*(long long)dy)/dx;
				xa = LeftEdge;
			}
		}

		DrawWideLine((long)(xa-WindowStartX32+Scale32Div2)/(long)Scale32,
					(long)(ya-WindowStartY32+Scale32Div2)/(long)Scale32,
					(long)(xb-WindowStartX32+Scale32Div2)/(long)Scale32,
					(long)(yb-WindowStartY32+Scale32Div2)/(long)Scale32,
					thickness/Scale32, fillType);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawGridDots32                                                       */
/*                                                                        */
/* ====================================================================== */

void DrawGridDots32(long spacing, long divisor)
{
	long dx;
	long dy;
	long x;
	long y;

	BeginDraw();
	for (dy=spacing*((divisor*WindowStartY32)/spacing);;dy+=spacing) {
		y=dy/divisor;
		if (y>WindowEndY32)
			break;
		y -= WindowStartY32;
		if (y>=0) {
			y = (y+Scale32Div2)/Scale32;
			for (dx=spacing*((divisor*WindowStartX32)/spacing);;dx+=spacing) {
				x = dx/divisor;
				if (x>WindowEndX32)
					break;
				x -= WindowStartX32;
				if (x>=0) {
					x = (x+Scale32Div2)/Scale32;
					PutPixel(x+WindowOriginX,y+WindowOriginY);
				}
			}
		}
	}
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLine                                                         */
/*                                                                        */
/* ====================================================================== */

static short radius;
static short x, y;

#define squared(x) ((long)x*x)

static long Hypotenuse2(short x, short y)
{
	return squared(x)+squared(y);
}

static void LeastSquare(void)
{
	short stepX;
	short stepY;

	long A1;
	long A2;
	long Square;

	short FitX = 0;
	short FitY = 0;

	A1 = Square = squared(radius);

	for (stepX=0;stepX<=1;stepX++) {
		for (stepY=0;stepY<=1;stepY++) {

			A2 = Hypotenuse2(x+stepX, y+stepY);

			if (Square==A2) {
				x += stepX;
				y += stepY;
				return;
			}

			if (abs(Square-A2)<=A1) {
				A1 = abs(Square-A2);
				FitX  = stepX;
				FitY  = stepY;
			}
		}
	}
	x += FitX;
	y += FitY;
}

void DrawWideLine(short xa, short ya, short xb, short yb,
	short thickness, unsigned char fillmode)
{
	short savedX;
	short savedY;

	short dx;
	short dy;

	short stepX;
	short stepY;
	short frac;

	long Square;

	short x1;
	short y1;
	short x2;
	short y2;
	short x3;
	short y3;
	short x4;
	short y4;
	short x5;
	short y5;
	short x6;
	short y6;

	LineDrawingMode = SOLID;
	if (fillmode&4) {
		LineDrawingMode = DASHED;
	}

	fillmode &= 3;

	savedX = xb;
	savedY = yb;

	thickness |= 1;
	radius = thickness/2;

	if (radius==0) {
		DrawLineFunction(xa, ya, xb, yb);
	} else {
		if ((min(xa,xb)-radius<=WindowEndX)
		 && (max(xa,xb)+radius>=WindowStartX)
		 && (min(ya,yb)-radius<=WindowEndY)
		 && (max(ya,yb)+radius>=WindowStartY)) {
			if (ya==yb) {
				if (xb<xa) {
					xb = xa;
					xa = savedX;
				}
				switch (fillmode) {
				case 0:
					DrawingPositionX = xa;
					DrawingPositionY = ya-radius;
					DrawSolidLineRelative(xb-xa, 0);
					DrawSolidLineRelative(0, radius*2);
					DrawSolidLineRelative(xa-xb, 0);
					DrawSolidLineRelative(0, -(radius*2));
					break;
				case 3:
					DrawCircleAbsolute((xa+xb)/2, ya, radius+(abs(xb-xa)/2),radius,0);
					break;
				default:
					FillRectangle(xa,ya-radius,xb,yb+radius);
					break;
				}
			} else
			if (xa==xb) {
				if (yb<ya) {
					yb = ya;
					ya = savedY;
				}
				switch (fillmode) {
				case 0:
					DrawingPositionX = xa-radius;
					DrawingPositionY = ya;
					DrawSolidLineRelative(0, yb-ya);
					DrawSolidLineRelative(radius*2, 0);
					DrawSolidLineRelative(0, ya-yb);
					DrawSolidLineRelative(-radius*2, 0);
					break;
				case 3:
					DrawCircleAbsolute(xa, (ya+yb)/2, radius,radius+(abs(yb-ya)/2),0);
					break;
				default:
					FillRectangle(xa-radius, ya, xb+radius, yb);
					break;
				}
			} else {
				if (xa>xb) {
					xb = xa;
					xa = savedX;
					yb = ya;
					ya = savedY;
				}

				if (abs(xa-ya)==abs(xb-yb)) {
					x = y = (7071L*thickness)/20000;
				} else {
					Square = squared(thickness);

					dx = abs(xb-xa)<<1;
					dy = abs(yb-ya)<<1;

					x = 0;
					y = 0;

					if (dx>dy) {
						frac = -(dx>>1);
						while (Hypotenuse2(x,y)<Square) {
							frac += dy;
							if (frac>=0) {
								x++;
								frac -= dx;
							}
							y++;
						}
					} else {
						frac = -(dy>>1);
						while (Hypotenuse2(x,y)<Square) {
							frac += dx;
							if (frac>=0) {
								y++;
								frac -= dy;
							}
							x++;
						}
					}
					x /= 2;
					y /= 2;
				}

				if (fillmode==3) {
					LeastSquare();
				}

				if (ya>yb) {
					x5 = xa;
					y5 = ya;
					x6 = xb;
					y6 = yb;

					x1 = xa + x;
					y1 = ya + y;

					x2 = xa - x;
					y2 = ya - y;

					x3 = xb + x;
					y3 = yb + y;

					x4 = xb - x;
					y4 = yb - y;
				} else {
					x5 = xb;
					y5 = yb;
					x6 = xa;
					y6 = ya;

					x1 = xb - x;
					y1 = yb + y;

					x2 = xa - x;
					y2 = ya + y;

					x3 = xb + x;
					y3 = yb - y;

					x4 = xa + x;
					y4 = ya - y;
				}

				switch (fillmode) {
				case 1:
				case 2:
					dx = x5-x6;
					dy = y5-y6;

					if (dx < 0) {
						dx = -dx;
						stepX = -1;
						x5 = x2;
						y5 = y2;
						x6 = x3;
						y6 = y3;
					} else {
						stepX =  1;
						x5 = x3;
						y5 = y3;
						x6 = x2;
						y6 = y2;
					}

					stepY = -1;

					dx <<= 1;
					dy <<= 1;

					if (dx < dy) {
						if (stepX == 1) {
							stepY = -stepY;
							x = x6; x6 = x4; x4 = x;
							y = y6; y6 = y4; y4 = y;
							x = x1; x1 = x5; x5 = x;
							y = y1; y1 = y5; y5 = y;
						}

						frac = -(dy >> 1);
						for (;;x1--,x6--) {
							DrawLineFunction(x1,y1,x6,y6);
							if (x6==x4)
								break;
							frac += dx;
							if (frac >= 0) {
								frac -= dy;
								y1 += stepY;
								y6 += stepY;
								DrawLineFunction(x1,y1,x6,y6);
							}
						}
					} else {
						frac = -(dx >> 1);
						for (;;y1--,y6--) {
							DrawLineFunction(x1,y1,x6,y6);
							if (y6==y4)
								break;
							frac += dy;
							if (frac >= 0) {
								frac -= dx;
								x1 += stepX;
								x6 += stepX;
								DrawLineFunction(x1,y1,x6,y6);
							}
						}
					}
					break;
				case 3:
					DrawArcAbsolute(x5,y5,
									x1,y1,
									x5,y5+radius,
									radius, 1);
					DrawArcAbsolute(x6,y6,
									x4,y4,
									x6,y6-radius,
									radius, 1);

					if (x6>=x5) {
						DrawArcAbsolute(x5,y5,
										x5,y5+radius,
										x5-radius,y5,
										radius, 1);
						DrawArcAbsolute(x5,y5,
										x2,y2,
										x5-radius,y5,
										radius, 1);
						DrawArcAbsolute(x6,y6,
										x3,y3,
										x6+radius,y6,
										radius, 1);
						DrawArcAbsolute(x6,y6,
										x6+radius,y6,
										x6,y6-radius,
										radius, 1);
						DrawLineFunction(x1,y1,x3,y3);
						DrawLineFunction(x2,y2,x4,y4);

					} else {
						DrawArcAbsolute(x5,y5,
										x5,y5+radius,
										x5+radius,y5,
										radius, 1);
						DrawArcAbsolute(x5,y5,
										x3,y3,
										x5+radius,y5,
										radius, 1);
						DrawArcAbsolute(x6,y6,
										x2,y2,
										x6-radius,y6,
										radius, 1);
						DrawArcAbsolute(x6,y6,
										x6-radius,y6,
										x6,y6-radius,
										radius, 1);
						DrawLineFunction(x1,y1,x2,y2);
						DrawLineFunction(x3,y3,x4,y4);
					}
					break;
				default:
					DrawLineFunction(x1,y1,x2,y2);
					DrawLineFunction(x2,y2,x4,y4);
					DrawLineFunction(x4,y4,x3,y3);
					DrawLineFunction(x3,y3,x1,y1);
					break;
				}
			}

			if (fillmode==2) {
				if (radius>=2) {
					DrawCircleAbsolute(xb,yb,radius,radius,1);
					DrawCircleAbsolute(xa,ya,radius,radius,1);
				}
			}
		}
	}

	DrawingPositionX = savedX;
	DrawingPositionY = savedY;
	LineDrawingMode = SOLID;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawLineFunction                                                     */
/*                                                                        */
/* ====================================================================== */

void DrawLineFunction(short xa, short ya, short xb, short yb)
{
	if (LineDrawingMode == SOLID) {
		DrawSolidLineAbsolute(xa, ya, xb, yb);
	} else if (LineDrawingMode == DASHED) {
		DrawDashedLineAbsolute(xa, ya, xb, yb);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   ClipLine                                                             */
/*                                                                        */
/* ====================================================================== */

bool ClipLine(short *const xa, short *const ya,
	short *const xb, short *const yb)
{
	short dx;
	short dy;
	short t;
	unsigned char outcode;
	unsigned char outcodeA;
	unsigned char outcodeB;

	dx = (*xb - *xa);
	dy = (*yb - *ya);

	if (dx == 0) {
		if ((*xa < WindowStartX) || (*xa > WindowEndX)) {
			return false;
		}

		if (*ya > *yb) {
			t = *ya; *ya = *yb; *yb = t;
		}

		if ((*ya > WindowEndY) || (*yb < WindowStartY)) {
			return false;
		}

		if (*ya < WindowStartY) {
			*ya = WindowStartY;
		}

		if (*yb > WindowEndY) {
			*yb = WindowEndY;
		}
	} else if (dy == 0) {
		if ((*ya < WindowStartY) || (*ya > WindowEndY)) {
			return false;
		}

		if (*xa > *xb) {
			t = *xa; *xa = *xb; *xb = t;
		}

		if ((*xa > WindowEndX) || (*xb < WindowStartX)) {
			return false;
		}

		if (*xa < WindowStartX) {
			*xa = WindowStartX;
		}

		if (*xb > WindowEndX) {
			*xb = WindowEndX;
		}
	} else {
		for (;;) {
			outcodeA = GetOutcode(*xa, *ya);
			outcodeB = GetOutcode(*xb, *yb);

			if ((outcodeA == INSIDE) && (outcodeB == INSIDE)) {
				break;
			} else if (outcodeA & outcodeB) {
				return false;
			}

			if (outcodeA == INSIDE) {
				t = *xa; *xa = *xb; *xb = t;
				t = *ya; *ya = *yb; *yb = t;
				outcode = outcodeB;
			} else {
				outcode = outcodeA;
			}

			dx = (*xb - *xa);
			dy = (*yb - *ya);

			if (outcode & LEFT) {
				*ya += ((long)dy * (WindowStartX - *xa)) / dx;
				*xa = WindowStartX;
			} else if (outcode & RIGHT) {
				*ya += ((long)dy * (WindowEndX - *xa)) / dx;
				*xa = WindowEndX;
			} else if (outcode & TOP) {
				*xa += ((long)dx * (WindowStartY - *ya)) / dy;
				*ya = WindowStartY;
			} else if (outcode & BOTTOM) {
				*xa += ((long)dx * (WindowEndY - *ya)) / dy;
				*ya = WindowEndY;
			}
		}
	}

	Translate(xa, ya);
	Translate(xb, yb);
	return true;
}

/* ====================================================================== */
/*                                                                        */
/*   GetOutcode                                                           */
/*                                                                        */
/* ====================================================================== */

unsigned char GetOutcode(short x, short y)
{
	unsigned char outcode = INSIDE;

	if (x < WindowStartX) {
		outcode |= LEFT;
	} else if (x > WindowEndX) {
		outcode |= RIGHT;
	}

	if (y < WindowStartY) {
		outcode |= TOP;
	} else if (y > WindowEndY) {
		outcode |= BOTTOM;
	}

	return outcode;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawSolidLine                                                        */
/*                                                                        */
/* ====================================================================== */

void DrawSolidLine(unsigned short xa, unsigned short ya,
	unsigned short xb, unsigned short yb)
{
	BeginDraw();
	MoveToEx(hdcMemoryBuffer, xa, ya, NULL);
	LineTo(hdcMemoryBuffer, xb, yb);
	SetPixel(hdcMemoryBuffer, xb, yb, RGBPalette[MapMask]);
	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
	{
		MoveToEx(hdcWindow, xa, ya, NULL);
		LineTo(hdcWindow, xb, yb);
		SetPixel(hdcWindow, xb, yb, RGBPalette[MapMask]);
	}
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   DrawDashedLine                                                       */
/*                                                                        */
/* ====================================================================== */

void DrawDashedLine(unsigned short xa, unsigned short ya,
	unsigned short xb, unsigned short yb)
{
	HPEN hDashPen = CreatePen(PS_DOT, 0, RGBPalette[MapMask]);
	HPEN hOldPen = SelectObject(hdcMemoryBuffer, hDashPen);
	int OldBkMode = SetBkMode(hdcMemoryBuffer, TRANSPARENT);
	SelectObject(hdcWindow, hDashPen);
	SetBkMode(hdcWindow, TRANSPARENT);
	DrawSolidLine(xa, ya, xb, yb);
	SelectObject(hdcMemoryBuffer, hOldPen);
	SelectObject(hdcWindow, hOldPen);
	SetBkMode(hdcMemoryBuffer, OldBkMode);
	SetBkMode(hdcWindow, OldBkMode);
	DeleteObject(hDashPen);
}

/* -- END --------------------------------------------------------------- */
