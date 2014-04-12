/* ********************************************************************** */
/*                                                                        */
/*   TEXT.H                                                               */
/*                                                                        */
/* ********************************************************************** */

#ifndef __TEXT_H__
#define __TEXT_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "driver.h"
#include "font.h"
#include "line.h"
#include "resolutn.h"
#include "state.h"
#include "video.h"

/* -- PRAGMAS ----------------------------------------------------------- */

/*
#pragma aux (DRIVER) PrintDriverInfo;
#pragma aux (DRIVER) DrawFixedStringAbsolute;
#pragma aux (DRIVER) DrawFixedStringRelative;
#pragma aux (DRIVER) DrawVariableStringAbsolute;
#pragma aux (DRIVER) DrawVariableStringRelative;
#pragma aux (DRIVER) DrawFixedTextAbsolute;
#pragma aux (DRIVER) DrawFixedTextRelative;
#pragma aux (DRIVER) DrawVariableTextAbsolute;
#pragma aux (DRIVER) DrawVariableTextRelative;
*/

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void PrintDriverInfo(void);
extern void DrawFixedStringAbsolute(unsigned short length,
	const unsigned char *string, short x, short y);
extern void DrawFixedStringRelative(unsigned short length,
	const unsigned char *string, short dx, short dy);
extern void DrawVariableStringAbsolute(unsigned short length,
	const unsigned char *string, short x, short y, short scale);
extern void DrawVariableStringRelative(unsigned short length,
	const unsigned char *string, short dx, short dy, short scale);
extern void DrawFixedTextAbsolute(const unsigned char *text,
	short x, short y);
extern void DrawFixedTextRelative(const unsigned char *text,
	short dx, short dy);
extern void DrawVariableTextAbsolute(const unsigned char *text,
	short x, short y, short scale);
extern void DrawVariableTextRelative(const unsigned char *text,
	short dx, short dy, short scale);

#endif

/* -- END --------------------------------------------------------------- */
