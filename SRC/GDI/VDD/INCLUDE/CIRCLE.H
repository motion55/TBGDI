/* ********************************************************************** */
/*                                                                        */
/*   CIRCLE.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __CIRCLE_H__
#define __CIRCLE_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "state.h"
#include "line.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) DrawArcAbsolute;
#pragma aux (DRIVER) DrawWideArcAbsolute32;
#pragma aux (DRIVER) DrawCircleAbsolute;
#pragma aux (DRIVER) DrawCircleRelative;
#pragma aux (DRIVER) DrawCircleAbsolute32;
*/

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void DrawArcAbsolute(short x, short y, short endxa,
	short endya, short endxb, short endyb, unsigned short radius,
	unsigned short thickness);
extern void DrawWideArcAbsolute32(long xc, long yc, long xa, long ya,
	long xb, long yb, unsigned long radius, unsigned long thickness,
	const unsigned char fillMode);
extern void DrawCircleAbsolute(short x, short y,
	unsigned short xsize, unsigned short ysize, unsigned char fillStyle);
extern void DrawCircleRelative(short dx, short dy,
	unsigned short xsize, unsigned short ysize, unsigned char fill);
extern void DrawCircleAbsolute32(long xc, long yc, long radius,
	unsigned char fillType);

#endif

/* -- END --------------------------------------------------------------- */
