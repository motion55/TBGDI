/* ********************************************************************** */
/*                                                                        */
/*   LINE.H                                                               */
/*                                                                        */
/* ********************************************************************** */

#ifndef __LINE_H__
#define __LINE_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include "global.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) DrawWideLineAbsolute;
#pragma aux (DRIVER) DrawWideLineRelative;
#pragma aux (DRIVER) DrawDashedLineAbsolute;
#pragma aux (DRIVER) DrawDashedLineRelative;
#pragma aux (DRIVER) DrawSolidLineAbsolute;
#pragma aux (DRIVER) DrawSolidLineRelative;
#pragma aux (DRIVER) DrawLineFunction;
*/

/* -- MACROS ------------------------------------------------------------ */

#define INSIDE	0
#define LEFT	1
#define RIGHT	2
#define TOP	4
#define BOTTOM	8

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void DrawWideLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawWideLineRelative(short dx, short dy);
extern void DrawDashedLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawDashedLineRelative(short dx, short dy);
extern void DrawSolidLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawSolidLineRelative(short dx, short dy);
extern void DrawWideLineAbsolute32(long xa, long ya, long xb, long yb,
	long thickness, unsigned char fillType);
extern void DrawLineFunction(short xa, short ya, short xb, short yb);

#endif

/* -- END --------------------------------------------------------------- */
