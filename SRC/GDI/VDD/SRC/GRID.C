/* ********************************************************************** */
/*                                                                        */
/*   GRID.C                                                               */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "grid.h"

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   DrawGridDots                                                         */
/*                                                                        */
/* ====================================================================== */

void DrawGridDots(unsigned short spacing)
{
	int i;
	int j;
	unsigned short x;
	unsigned short y;
	unsigned short ndotsx;
	unsigned short ndotsy;

	ndotsx = ((WindowEndX - WindowStartX) / spacing) + 1;
	ndotsy = ((WindowEndY - WindowStartY) / spacing) + 1;

	y = WindowOriginY;

	BeginDraw();
	for (j = 0; j < ndotsy; j++) {
		x = WindowOriginX;

		for (i = 0; i < ndotsx; i++) {
			PutPixel(x, y);
			x += (spacing / Scale);
		}

		y += (spacing / Scale);
	}
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   DrawGridDotsRelative                                                 */
/*                                                                        */
/* ====================================================================== */

void DrawGridDotsRelative(unsigned short spacing, unsigned short dx,
	unsigned short dy)
{
	int i;
	int j;
	unsigned short x;
	unsigned short y;
	unsigned short ndotsx;
	unsigned short ndotsy;

	ndotsx = ((WindowEndX - WindowStartX - dx) / spacing) + 1;
	ndotsy = ((WindowEndY - WindowStartY - dy) / spacing) + 1;

	y = WindowOriginY + (dy / Scale);

	BeginDraw();
	for (j = 0; j < ndotsy; j++) {
		x = WindowOriginX + (dx / Scale);

		for (i = 0; i < ndotsx; i++) {
			PutPixel(x, y);
			x += (spacing / Scale);
		}

		y += (spacing / Scale);
	}
	EndDraw();
}

/* -- END --------------------------------------------------------------- */
