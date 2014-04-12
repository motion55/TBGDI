/* ********************************************************************** */
/*                                                                        */
/*   GRID.H                                                               */
/*                                                                        */
/* ********************************************************************** */

#ifndef __GRID_H__
#define __GRID_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "state.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */
/*
#pragma aux (DRIVER) DrawGridDots;
#pragma aux (DRIVER) DrawGridDotsRelative;
*/
/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void DrawGridDots(unsigned short spacing);
extern void DrawGridDotsRelative(unsigned short spacing, unsigned short dx,
	unsigned short dy);

#endif

/* -- END --------------------------------------------------------------- */
