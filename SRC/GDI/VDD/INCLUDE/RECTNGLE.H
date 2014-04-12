/* ********************************************************************** */
/*                                                                        */
/*   RECTNGLE.H                                                           */
/*                                                                        */
/* ********************************************************************** */

#ifndef __RECTNGLE_H__
#define __RECTNGLE_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include "global.h"
#include "resolutn.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) ClearRectangle;
#pragma aux (DRIVER) FillRectangle;
#pragma aux (DRIVER) HighlightRectangle;
#pragma aux (DRIVER) SaveRectangle;
#pragma aux (DRIVER) RestoreRectangle;
#pragma aux (DRIVER) CopyRectangle;
*/

/* -- PUBLIC DATA DECLARATIONS ------------------------------------------ */

extern HDC hdcSavedRect;
extern HBITMAP hbmSavedRect;

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void ClearRectangle(short xa, short ya, short xb, short yb);
extern void FillRectangle(short xa, short ya, short xb, short yb);
extern void HighlightRectangle(short xa, short ya, short xb, short yb);
extern void SaveRectangle(short xa, short ya, short xb, short yb);
extern void RestoreRectangle(void);
extern void CopyRectangle(short xa, short ya, short xb, short yb);

#endif

/* -- END --------------------------------------------------------------- */
