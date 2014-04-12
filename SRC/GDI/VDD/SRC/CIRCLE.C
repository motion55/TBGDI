/* ********************************************************************** */
/*                                                                        */
/*   CIRCLE.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "circle.h"

/* -- PRIVATE FUNCTION DECLARATIONS ------------------------------------- */

static unsigned char GetQuadrant(long xc, long yc,
	long *const x, long *const y, long radius);
static void DrawArc(long xc, long yc, long xa, long ya, long xb, long yb,
	long radius, long thickness);
static void PutPixel32(long x, long y);
static void DrawCircleOctants(short dx, short dy, unsigned short distance,
	int type);
static void FillCircleOctants(short dx, short dy, unsigned short distance,
	int type);
static void DrawCirclePoint(short dx, short dy);
static void DrawCircleLine(short dx, short dy);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   DrawArcAbsolute                                                      */
/*                                                                        */
/* ====================================================================== */

void DrawArcAbsolute(short xc, short yc, short endxa, short endya,
	short endxb, short endyb, unsigned short radius,
	unsigned short thickness)
{
	short mulX;
	short mulY;
	short radiusi, radiuso;
	short lxa;
	short lya;
	short lxb;
	short lyb;
	short eps;
	short u;
	short v;
	short r;
	short s;
	short t;
	short x,y;
	char draw_beg, draw_end;

	if (((min(endxa, endxb) - thickness) > WindowEndX)
	 || ((max(endxa, endxb) + thickness) < WindowStartX)
	 || ((min(endya, endyb) - thickness) > WindowEndY)
	 || ((max(endya, endyb) + thickness) < WindowStartY)) {
		return;
	}

	if (thickness == 0) {
		thickness = 1;
	}

	if (radius == 0) {
		radius = 1;
	}

	endxa -= xc;
	endya -= yc;
	endxb -= xc;
	endyb -= yc;

	if ((endxa < 0) || (endxb < 0))
		mulX = -Scale;
	else
		mulX = Scale;

	if ((endya < 0) || (endyb < 0))
		mulY = -Scale;
	else
		mulY = Scale;

	endxa = abs(endxa);
	endya = abs(endya);
	endxb = abs(endxb);
	endyb = abs(endyb);

	if (thickness == 1) {
		eps = 2 * (radius - 1);
		u = 4 * radius;
		v = 0;

		lxa = radius;
		lya = 0;

		draw_beg = 0;
		draw_end = 0;

		BeginDraw();
		while (lxa >= 0) {
			if ((endxa==lxa)&&(endya==lya)) {
				if (draw_beg)
					draw_end = -1;
				else
					draw_beg = -1;
			}

			if ((endxb==lxa)&&(endyb==lya)) {
				if (draw_beg)
					draw_end = -1;
				else
					draw_beg = -1;
			}

			if (draw_beg) {
				if (mulX<0)
					x = xc - lxa;
				else
					x = xc + lxa;

				if (mulY<0)
					y = yc - lya;
				else
					y = yc + lya;

				if ((x >= WindowStartX) && (x <= WindowEndX)
				&&  (y >= WindowStartY) && (y <= WindowEndY)) {
					Translate(&x, &y);
					PutPixel(x, y);
				}

				if (draw_end)
					break;
			}

			if (eps > 0) {
				lya++;
				v += 4;
				eps -= v;
			} else {
				lxa--;
				u -= 4;
				eps += u;
			}
		}
		EndDraw();
	} else {
		lxa = radiusi = radius - (thickness / 2);
		lya = 0;
		lxb = radiuso = radius + (thickness / 2);
		lyb = 0;

		s   = (lxa - 1) * 2;
		r   = (s + 2) * 2;
		t   = 0;

		eps = (lxb - 1) * 2;
		u   = (eps + 2) * 2;
		v   = 0;

		DrawLineFunction((lxa + mulX) + xc, (lya + mulY) + yc,
						(lxb + mulX) + xc, (lyb + mulY) + yc);

		while (lyb != radiuso) {

			while (eps<=0) {
			   lxb--;
			   u -= 4;
			   eps += u;
			}

			lyb++;
			v += 4;
			eps -= v;

			if (lya<radiusi) {
				if (s>0) {
					lya++;
					t += 4;
					s -= t;
				}
				while (s<=0) {
					lxa--;
					if (lxa<=0) {
						lxa = 0;
						break;
					}
					r -= 4;
					s += r;
				}
			} else {
			   lya++;
			}

			DrawLineFunction((lxa * mulX) + xc, (lya * mulY) + yc,
				(lxb * mulX) + xc, (lyb * mulY) + yc);
		}
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideArcAbsolute32                                                */
/*                                                                        */
/* ====================================================================== */

void DrawWideArcAbsolute32(long xc, long yc, long xa, long ya,
	long xb, long yb, unsigned long radius, unsigned long thickness,
	const unsigned char fillMode)
{
	unsigned char quadrants;
	long t;
	long xbi;
	long xbo;
	long xarc;
	long yai;
	long yao;
	long yarc;

	if (((fillMode == 0) || (fillMode == 3)) && (thickness >= 3)) {
		quadrants = GetQuadrant(xc, yc, &xa, &ya, radius);
		quadrants |= GetQuadrant(xc, yc, &xb, &yb, radius) << 2;

		if ((quadrants == 4)  || (quadrants == 6)
		||  (quadrants == 12) || (quadrants == 14)) {
			t = xa; xa = xb; xb = t;
			t = ya; ya = yb; yb = t;
		}

		switch (quadrants) {
			case 1:
			case 4:
				xbi = xb - (thickness / 2);
				xbo = xb + (thickness / 2);
				xarc = xa - (thickness / 2);
				yai = ya + (thickness / 2);
				yao = ya - (thickness / 2);
				yarc = yb + (thickness / 2);
				break;
			case 6:
			case 9:
				xbi = xb + (thickness / 2);
				xbo = xb - (thickness / 2);
				xarc = xa + (thickness / 2);
				yai = ya + (thickness / 2);
				yao = ya - (thickness / 2);
				yarc = yb + (thickness / 2);
				break;
			case 11:
			case 14:
				xbi = xb + (thickness / 2);
				xbo = xb - (thickness / 2);
				xarc = xa + (thickness / 2);
				yai = ya - (thickness / 2);
				yao = ya + (thickness / 2);
				yarc = yb - (thickness / 2);
				break;
			case 3:
			case 12:
				xbi = xb - (thickness / 2);
				xbo = xb + (thickness / 2);
				xarc = xa - (thickness / 2);
				yai = ya - (thickness / 2);
				yao = ya + (thickness / 2);
				yarc = yb - (thickness / 2);
				break;
			default:
				return;
		}

		DrawArc(xc, yc, xa, yai, xbo, yb, radius - (thickness / 2), 1);
		DrawArc(xc, yc, xa, yao, xbo, yb, radius + (thickness / 2), 1);

		if (fillMode == 3) {
			DrawArc(xc, ya, xc, yai, xarc, ya, (thickness / 2), 1);
			DrawArc(xc, ya, xarc, ya, xc, yao, (thickness / 2), 1);
			DrawArc(xb, yc, xbi, yc, xb, yarc, (thickness / 2), 1);
			DrawArc(xb, yc, xb, yarc, xbo, yc, (thickness / 2), 1);
		} else {
			DrawWideLineAbsolute32(xbi, yc, xbo, yc, 0, 0);
			DrawWideLineAbsolute32(xc, yai, xc, yao, 0, 0);
		}
	} else {
		DrawArc(xc, yc, xa, ya, xb, yb, radius, thickness);

		if (fillMode == 2) {
			DrawCircleAbsolute32(xa, ya, (thickness / 2), 1);
			DrawCircleAbsolute32(xb, yb, (thickness / 2), 1);
		}
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleAbsolute                                                   */
/*                                                                        */
/* ====================================================================== */

void DrawCircleAbsolute(short x, short y,
	unsigned short xsize, unsigned short ysize, unsigned char fill)
{
	int type;
	unsigned short rad;
	unsigned short dist;
	short dx;
	short dy;
	short eps;
	short u;
	short v;

	DrawingPositionX = x;
	DrawingPositionY = y;

	if (((signed)(x - xsize) > WindowEndX)
	 || ((signed)(x + xsize) < WindowStartX)
	 || ((signed)(y - ysize) > WindowEndY)
	 || ((signed)(y + ysize) < WindowStartY)) {
		return;
	}

	if (Scale > 1) {
		xsize /= Scale;
		ysize /= Scale;
	}

	if ((xsize == 0) || (ysize == 0)) {
		return;
	}

	rad = min(xsize, ysize);
	dist = abs(xsize - ysize);

	if (xsize == ysize) {
		type = 0;
	}
	else if (xsize > ysize) {
		type = 1;
		DrawLineFunction((x - dist), (y - rad),
			(x + dist), (y - rad));
		DrawLineFunction((x - dist), (y + rad),
			(x + dist), (y + rad));
	}
	else
	{
		type = -1;
		DrawLineFunction((x - rad), (y - dist),
			(x - rad), (y + dist));
		DrawLineFunction((x + rad), (y - dist),
			(x + rad), (y + dist));
	}

	DrawingPositionX = x;
	DrawingPositionY = y;

	dx = 0;
	dy = rad;
	eps = 1 - rad;
	u = 1;
	v = eps - rad;

	if (fill != 0) {
		while(dx <= dy) {
			FillCircleOctants(dx, dy, dist, type);
			u += 2;

			if (eps < 0) {
				v += 2;
				eps += u;
			} else {
				v += 4;
				eps += v;
				dy--;
			}

			dx++;
		}
	} else {

		while(dx <= dy) {
			DrawCircleOctants(dx, dy, dist, type);
			u += 2;

			if (eps < 0) {
				v += 2;
				eps += u;
			} else {
				v += 4;
				eps += v;
				dy--;
			}

			dx++;
		}
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleRelative                                                   */
/*                                                                        */
/* ====================================================================== */

void DrawCircleRelative(short dx, short dy, unsigned short radiusX,
	unsigned short radiusY,	unsigned char fillStyle)
{
	DrawCircleAbsolute((DrawingPositionX + dx), (DrawingPositionY + dy),
		radiusX, radiusY, fillStyle);
}

#if 1
/* ====================================================================== */
/*                                                                        */
/*   DrawCirclePoint32                                                    */
/*                                                                        */
/* ====================================================================== */

void DrawCirclePoint32(long x, long y)
{
	if ((x < WindowStartX32) || (x > WindowEndX32)
	||  (y < WindowStartY32) || (y > WindowEndY32)) {
		return;
	}

	x -= WindowStartX32;
	y -= WindowStartY32;

	x = ((long long)x + Scale32Div2) / Scale32;
	y = ((long long)y + Scale32Div2) / Scale32;

	x += WindowOriginX;
	y += WindowOriginY;

	PutPixel(x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleSector32                                                   */
/*                                                                        */
/* ====================================================================== */

void DrawCircleSector32(long xc, long yc, long dx, long dy, int sector,
	unsigned char fill)
{
	long t;

	switch (sector) {
		case 0:
			break;
		case 1:
			t = dx; dx = dy; dy = t;
			break;
		case 2:
			if (dx == 0) {
				return;
			}
			t = dx; dx = dy; dy = -t;
			break;
		case 3:
			dy = -dy;
			break;
	}

	if (fill == 0) {
		BeginDraw();
		DrawCirclePoint32((xc - dx), (yc - dy));

		if (dx != 0) {
			DrawCirclePoint32((xc + dx), (yc - dy));
		}
		EndDraw();
	} else {
		DrawWideLineAbsolute32((xc - dx), (yc - dy),
			(xc + dx), (yc - dy), 0, 0);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleAbsolute32                                                 */
/*                                                                        */
/* ====================================================================== */

void DrawCircleAbsolute32(long xc, long yc, long radius,
					unsigned char fillType)
{
	long dx;
	long dy;
	short x;
	short y;
	int eps;
	int u;
	int v;
	int sector;

	if (((xc - radius) > WindowEndX32)
	||  ((xc + radius) < WindowStartX32)
	||  ((yc - radius) > WindowEndY32)
	||  ((yc + radius) < WindowStartY32)) {
		return;
	}

	radius /= Scale32;
	if (radius == 0) {
		return;
	}

	x = DrawingPositionX;
	y = DrawingPositionY;

	for (sector = 0; sector < 4; sector++) {
		dx = 0;
		dy = (radius * Scale32);

		eps = 1 - radius;
		u = 1;
		v = 1 - (2 * radius);

		while(dx < dy) {
			if ((fillType == 0)
			||  ((fillType != 0) && ((sector == 1) || (sector == 2)))) {
				DrawCircleSector32(xc, yc, dx, dy, sector, fillType);
			}

			u += 2;
			if (eps < 0) {
				v += 2; eps += u;
			} else {
				if ((fillType != 0) && ((sector == 0) || (sector == 3))) {
					DrawCircleSector32(xc, yc, dx, dy, sector, fillType);
				}
				v += 4; eps += v;
				dy -= Scale32;
			}
			dx += Scale32;
		}

		if ((dx == dy) && ((sector == 0) || (sector == 3))) {
			DrawCircleSector32(xc, yc, dx, dy, sector, fillType);
		}
	}

	/*
	 * on exit, DrawingPositionX and DrawingPositionY may have values
	 * different to those of the reference implementation
	 */
	DrawingPositionX = x;
	DrawingPositionY = y;
}

#else

static long DrawingPositionX32;
static long DrawingPositionY32;

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleOctants32                                                  */
/*                                                                        */
/* ====================================================================== */

void DrawCircleOctants32(long dx, long dy)
{
	if (Scale32 > 1) {
		dx *= Scale32;
		dy *= Scale32;
	}

	PutPixel32(DrawingPositionX32+dx, DrawingPositionY32+dy);
	PutPixel32(DrawingPositionX32+dx, DrawingPositionY32-dy);
	PutPixel32(DrawingPositionX32-dx, DrawingPositionY32+dy);
	PutPixel32(DrawingPositionX32-dx, DrawingPositionY32-dy);
	PutPixel32(DrawingPositionX32+dy, DrawingPositionY32+dx);
	PutPixel32(DrawingPositionX32+dy, DrawingPositionY32-dx);
	PutPixel32(DrawingPositionX32-dy, DrawingPositionY32+dx);
	PutPixel32(DrawingPositionX32-dy, DrawingPositionY32-dx);
}

/* ====================================================================== */
/*                                                                        */
/*   FillCircleLine32                                                     */
/*                                                                        */
/* ====================================================================== */

void FillCircleLine32(long dx, long dy)
{
	long xn;
	long xp;
	long y;

	short savedX;
	short savedY;

	if (Scale32 > 1) {
		dx *= Scale32;
		dy *= Scale32;
	}

	xn = DrawingPositionX32 - dx;
	xp = DrawingPositionX32 + dx;

	if ((xp<WindowStartX32)
	 || (xn>WindowEndX32))
		return;

	if (xn<WindowStartX32)
		xn=WindowStartX32;
	if (xp>WindowEndX32)
		xp=WindowEndX32;

	savedX = DrawingPositionX;
	savedY = DrawingPositionY;

	xn = (xn-WindowStartX32+Scale32Div2)/Scale32;
	xp = (xp-WindowStartX32+Scale32Div2)/Scale32;

	if (dy==0) {
		y = (DrawingPositionY32-WindowStartY32+Scale32Div2)/Scale32;
		DrawSolidLineAbsolute(xn, y, xp, y);
	} else {
		y = (DrawingPositionY32-WindowStartY32+Scale32Div2-dy)/Scale32;
		DrawSolidLineAbsolute(xn, y, xp, y);
		y = (DrawingPositionY32-WindowStartY32+Scale32Div2+dy)/Scale32;
		DrawSolidLineAbsolute(xn, y, xp, y);
	}

	DrawingPositionX = savedX;
	DrawingPositionY = savedY;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleAbsolute32                                                 */
/*                                                                        */
/* ====================================================================== */

void DrawCircleAbsolute32(long xc, long yc, long radius,
					unsigned char fillType)
{
	long dx;
	long dy;
	long eps;
	long u;
	long v;

	DrawingPositionX32 = xc;
	DrawingPositionY32 = yc;

	if (((xc-radius)>WindowEndX32)
	 || ((xc+radius)<WindowStartX32)
	 || ((yc-radius)>WindowEndY32)
	 || ((yc+radius)<WindowStartY32)) {
		return;
	}

	radius = radius/Scale32;
	if (radius==0)
		return;

	dx = 0;
	dy = radius;

	eps = 1 - radius;
	u = 1;
	v = eps - radius;

	if (fillType==0) {
		BeginDraw();
		while (dx <= dy) {
			DrawCircleOctants32(dx, dy);
			u += 2;
			if (eps < 0) {
				v += 2;
				eps += u;
			} else {
				v += 4;
				eps += v;
				dy--;
			}
			dx++;
		}
		EndDraw();
	} else {
		while (dx <= dy) {
			FillCircleLine32(dy, dx);
			u += 2;
			if (eps < 0) {
				v += 2;
				eps += u;
			} else {
				FillCircleLine32(dx, dy);
				v += 4;
				eps += v;
				dy--;
			}
			dx++;
		}
	}
}
#endif

/* ====================================================================== */
/*                                                                        */
/*   PutPixel32                                                           */
/*                                                                        */
/* ====================================================================== */

void PutPixel32(long x, long y)
{
	if ((x < WindowStartX32) || (x > WindowEndX32)
	||  (y < WindowStartY32) || (y > WindowEndY32)) {
		return;
	}

	x -= WindowStartX32;
	y -= WindowStartY32;

	x = (x + Scale32Div2) / Scale32;
	y = (y + Scale32Div2) / Scale32;

	PutPixel(WindowOriginX + x, WindowOriginY + y);
}

/* ====================================================================== */
/*                                                                        */
/*   GetQuadrant                                                          */
/*                                                                        */
/* ====================================================================== */

unsigned char GetQuadrant(long xc, long yc,
	long *const x, long *const y, long radius)
{
	unsigned char quadrant;

	if (*x == xc) {
		if (*y > yc) {
			quadrant = 3;
			*y = yc + radius;
		} else {
			quadrant = 1;
			*y = yc - radius;
		}
	} else {
		if (*x > xc) {
			quadrant = 2;
			*x = xc + radius;
		} else {
			quadrant = 0;
			*x = xc - radius;
		}
	}

	return quadrant;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawArc                                                              */
/*                                                                        */
/* ====================================================================== */

void DrawArc(long xc, long yc, long xa, long ya, long xb, long yb,
	long radius, long thickness)
{
	long mulX;
	long mulY;
	long lxa;
	long lya;
	long lxb;
	long lyb;
	long eps;
	long u;
	long v;
	long r;
	long s;
	long t;

	if (((max(xa, xb) + thickness) < WindowStartX32)
	||  ((min(xa, xb) - thickness) > WindowEndX32)
	||  ((max(ya, yb) + thickness) < WindowStartY32)
	||  ((min(ya, yb) - thickness) > WindowEndY32)) {
		return;
	}

	thickness = thickness / Scale32;
	if (thickness == 0) {
		thickness = 1;
	}

	if (radius < 0) {
		radius = -radius;
	}

	radius = radius / Scale32;
	if (radius == 0) {
		radius = 1;
	}

	if ((xa < xc) || (xb < xc)) {
		mulX = -Scale32;
	} else {
		mulX = Scale32;
	}

	if ((ya < yc) || (yb < yc)) {
		mulY = -Scale32;
	} else {
		mulY = Scale32;
	}

	if (thickness == 1) {
		eps = 2 * (radius - 1);
		u = 4 * radius;
		v = 0;

		lxa = radius;
		lya = 0;

		BeginDraw();
		while (lxa >= 0) {
			PutPixel32((lxa * mulX) + xc, (lya * mulY) + yc);

			if (eps > 0) {
				lya++;
				v += 4;
				eps -= v;
			} else {
				lxa--;
				u -= 4;
				eps += u;
			}
		}
		EndDraw();
	} else {
		lxa = radius - (thickness / 2);
		lya = 0;
		lxb = radius + (thickness / 2);
		lyb = 0;

		DrawWideLineAbsolute32(xc + (lxa * mulX), yc + (lya * mulY),
			xc + (lxb * mulX), yc + (lyb * mulY), 0, 0);

		eps = (lxb - 1) * 2;
		u = (eps + 2) * 2;
		v = 0;

		s = (lxa - 1) * 2;
		r = (s + 2) * 2;
		t = 0;

		while (lyb != (radius + (thickness / 2))) {
			for (;;) {
				if (eps > 0) {
					lyb++;
					v += 4;
					eps -= v;
					break;
				} else {
					lxb--;
					u -= 4;
					eps += u;
				}
			}

			if (lya >= (radius - (thickness / 2))) {
				lya++;
			} else {
				if (s > 0) {
					lya++;
					t += 4;
					s -= t;

				}

				for (;;) {
					if (s <= 0) {
						lxa--;

						if (lxa <= 0) {
							lxa = 0;
							break;
						}

						r -= 4;
						s += r;
					} else {
						break;
					}
				}
			}

			DrawWideLineAbsolute32((lxa * mulX) + xc, (lya * mulY) + yc,
				(lxb * mulX) + xc, (lyb * mulY) + yc, 0, 0);
		}
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleOctants                                                    */
/*                                                                        */
/* ====================================================================== */

void DrawCircleOctants(short dx, short dy, unsigned short distance, int type)
{
	BeginDraw();
	if (type == 0) {
		DrawCirclePoint(-dx, -dy);
		DrawCirclePoint(-dy, -dx);
		DrawCirclePoint(-dx,  dy);
		DrawCirclePoint(-dy,  dx);
		DrawCirclePoint( dx, -dy);
		DrawCirclePoint( dy, -dx);
		DrawCirclePoint( dx,  dy);
		DrawCirclePoint( dy,  dx);
	} else if (type == 1) {
		DrawCirclePoint((-dx - distance), -dy);
		DrawCirclePoint((-dy - distance), -dx);
		DrawCirclePoint((-dx - distance),  dy);
		DrawCirclePoint((-dy - distance),  dx);
		DrawCirclePoint(( dx + distance), -dy);
		DrawCirclePoint(( dy + distance), -dx);
		DrawCirclePoint(( dx + distance),  dy);
		DrawCirclePoint(( dy + distance),  dx);
	} else {
		DrawCirclePoint(-dx, (-dy - distance));
		DrawCirclePoint(-dy, (-dx - distance));
		DrawCirclePoint( dx, (-dy - distance));
		DrawCirclePoint( dy, (-dx - distance));
		DrawCirclePoint(-dx, ( dy + distance));
		DrawCirclePoint(-dy, ( dx + distance));
		DrawCirclePoint( dx, ( dy + distance));
		DrawCirclePoint( dy, ( dx + distance));
	}
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   FillCircleOctants                                                    */
/*                                                                        */
/* ====================================================================== */

void FillCircleOctants(short dx, short dy, unsigned short distance, int type)
{
	if (type == 0) {
		DrawCircleLine(dx, dy);
	} else if (type == 1) {
		DrawingPositionX -= distance;
		DrawCircleLine(dx, dy);
		DrawingPositionX += 2 * distance;
		DrawCircleLine(dx, dy);
		DrawingPositionX += distance;
	} else {
		DrawingPositionY -= distance;
		DrawCircleLine(dx, dy);
		DrawingPositionY += 2 * distance;
		DrawCircleLine(dx, dy);
		DrawingPositionY += distance;
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCirclePoint                                                      */
/*                                                                        */
/* ====================================================================== */

void DrawCirclePoint(short dx, short dy)
{
	short x;
	short y;

	x = dx;
	y = dy;

	if (Scale > 1) {
		x *= Scale;
		y *= Scale;
	}

	x += DrawingPositionX;
	y += DrawingPositionY;

	if ((x < WindowStartX) || (x > WindowEndX)
	||  (y < WindowStartY) || (y > WindowEndY)) {
		return;
	}

	Translate(&x, &y);
	PutPixel(x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleLine                                                       */
/*                                                                        */
/* ====================================================================== */

void DrawCircleLine(short dx, short dy)
{
	short x;
	short y;

	if (Scale > 1) {
		dx *= Scale;
		dy *= Scale;
	}

	x = DrawingPositionX;
	y = DrawingPositionY;

	DrawSolidLineAbsolute((x - dx), (y - dy), (x + dx), (y - dy));
	DrawSolidLineAbsolute((x - dx), (y + dy), (x + dx), (y + dy));
	DrawSolidLineAbsolute((x - dy), (y - dx), (x + dy), (y - dx));
	DrawSolidLineAbsolute((x - dy), (y + dx), (x + dy), (y + dx));

	DrawingPositionX = x;
	DrawingPositionY = y;
}

/* -- END --------------------------------------------------------------- */
