/* ********************************************************************** */
/*                                                                        */
/*   CURSOR.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __CURSOR_H__
#define __CURSOR_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include "global.h"
#include "state.h"
#include "video.h"
#include "resolutn.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) SetCursorState;
#pragma aux (DRIVER) SetCursorStyle;
*/

/* -- PUBLIC DATA DECLARATIONS ------------------------------------------ */

#define CURSOR_HSIZE	8
#define CURSOR_VSIZE	15

extern unsigned char CursorStyle;
extern unsigned char CursorState;

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void InitCursor(void);
extern void UnloadCursor(void);
extern void SetCursorState(unsigned char state, short x, short y);
extern unsigned char SetCursorStyle(unsigned char style);

extern void ZoomScroll(void);

#endif

/* -- END --------------------------------------------------------------- */
