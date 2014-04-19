/* ********************************************************************** */
/*                                                                        */
/*   BITMAP.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "bitmap.h"

/* -- CODE -------------------------------------------------------------- */

static short clipxa, clipya, clipxb, clipyb;

static void ClpPixel(short x, short y)
{
	if (x >= clipxa && x <= clipxb && y >= clipya && y <= clipyb)
	{
		PUTPIXEL(x, y);
	}
}

static void Translate2(short *const x, short *const y)
{
	*x -= WindowStartX;
	*y -= WindowStartY;

	if (Scale > 1) {
		*x /= (short)Scale;
		*y /= (short)Scale;
	}

	*x += WindowOriginX;
	*y += WindowOriginY;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawBitmapRelative                                                   */
/*                                                                        */
/* ====================================================================== */

void DrawBitmapRelative(const unsigned char *bitmap, unsigned short rows,
	unsigned short cols, short dx, short dy, unsigned short xsize,
	unsigned short ysize, unsigned char function)
{
	short xa, xb, ya, yb;
	unsigned int i, j;
	int b = 0;

	xa = DrawingPositionX = DrawingPositionX + dx;
	ya = DrawingPositionY = DrawingPositionY + dy;

	if (xsize | ysize == 0)
	{
		xsize = cols * 8;
		ysize = rows;
	}

	xb = xa + ((function & 1)? ysize : xsize) * Scale;
	yb = ya + ((function & 1)? xsize : ysize) * Scale;

	if (xa > WindowEndX || ya > WindowEndY || xb < WindowStartX || yb < WindowStartY)
	{
		return;
	}

	if (xb > WindowEndX || yb > WindowEndY || xa < WindowStartX || ya < WindowStartY)
	{
		clipxa = WindowStartX;
		clipya = WindowStartY;
		clipxb = WindowEndX;
		clipyb = WindowEndY;
		Translate(&clipxa, &clipya);
		Translate(&clipxb, &clipyb);
		function += 8;
	}
	Translate2(&xa, &ya);
	BeginDraw();
	for (i = 0; i < ysize; i++)
	{
		for (j = 0; j < xsize; j++)
		{
			if (*bitmap & (0x80 >> b))
			{
				switch (function)
				{
					case 0:  PUTPIXEL(xa +         j, ya +         i); break;
					case 1:  PUTPIXEL(xa +         i, ya + xsize - j); break;
					case 2:  PUTPIXEL(xa + xsize - j, ya + ysize - i); break;
					case 3:  PUTPIXEL(xa + ysize - i, ya +         j); break;
					case 4:  PUTPIXEL(xa + xsize - j, ya +         i); break;
					case 5:  PUTPIXEL(xa +         i, ya +         j); break;
					case 6:  PUTPIXEL(xa +         j, ya + ysize - i); break;
					case 7:  PUTPIXEL(xa + ysize - i, ya + xsize - j); break;

					case 8:  ClpPixel(xa +         j, ya +         i); break;
					case 9:  ClpPixel(xa +         i, ya + xsize - j); break;
					case 10: ClpPixel(xa + xsize - j, ya + ysize - i); break;
					case 11: ClpPixel(xa + ysize - i, ya +         j); break;
					case 12: ClpPixel(xa + xsize - j, ya +         i); break;
					case 13: ClpPixel(xa +         i, ya +         j); break;
					case 14: ClpPixel(xa +         j, ya + ysize - i); break;
					case 15: ClpPixel(xa + ysize - i, ya + xsize - j); break;
				}
			}
			if (++b == 8)
			{
				b = 0;
				++bitmap;
			}
		}
		if (b)
		{
			b = 0;
			++bitmap;
		}
	}
	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
		BitBlt(hdcWindow, xa, ya, xsize, ysize, hdcMemoryBuffer, xa, ya, SRCCOPY);
	EndDraw();
}

/* -- END --------------------------------------------------------------- */
