/* ********************************************************************** */
/*                                                                        */
/*   BITMAP.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __BITMAP_H__
#define __BITMAP_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "driver.h"
#include "state.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) DrawBitmapRelative;
*/

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void DrawBitmapRelative(const unsigned char *bitmap,
	unsigned short rows, unsigned short cols, short dx, short dy,
	unsigned short xsize, unsigned short ysize, unsigned char function);

#endif

/* -- END --------------------------------------------------------------- */
