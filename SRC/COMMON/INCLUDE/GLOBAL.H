/* ********************************************************************** */
/*                                                                        */
/*   GLOBAL.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <stdlib.h>
#include <i86.h>

/* -- PRAGMAS ---------------------------------------------------------- */

#pragma aux DRIVER "*" 		\
	parm reverse routine [] \
	modify [ax dx es] 	\
	loadds far;

/* -- MACROS ------------------------------------------------------------ */

#define NORMAL  0
#define AND	1
#define OR	2
#define XOR	3

#define READ	0
#define WRITE	1

#define SOLID	0
#define DASHED	1

/* -- TYPE DEFINITIONS -------------------------------------------------- */

typedef enum {false = 0, true = -1} bool;

enum DispatchCallNumber {
	dcnInitDriver = 0,
	dcnShutdownDriver,
	dcnPrintDriverInfo,
	dcnGetScreenResolution,
	dcnGetColor,
	dcnSetColor,
	dcnGetScale,
	dcnSetScale,
	dcnSetWindow,
	dcnSetWindowOrigin,
	dcnSetCursorState,
	dcnSetCursorStyle,
	dcnSaveWindowState,
	dcnRestoreWindowState,
	dcnDrawArcAbsolute,
	dcnDrawWideArcAbsolute32,
	dcnDrawWideLineAbsolute,
	dcnDrawWideLineRelative,
	dcnDrawCircleAbsolute,
	dcnDrawCircleRelative,
	dcnDrawDashedLineAbsolute,
	dcnDrawDashedLineRelative,
	dcnDrawGridDots,
	dcnDrawGridDotsRelative,
	dcnSetWindow32,
	dcnDrawSolidLineAbsolute,
	dcnDrawSolidLineRelative,
	dcnMoveAbsolute,
	dcnMoveRelative,
	dcnDrawBitmapRelative,
	dcnDrawFixedStringAbsolute,
	dcnDrawFixedStringRelative,
	dcnDrawVariableStringAbsolute,
	dcnDrawVariableStringRelative,
	dcnDrawFixedTextAbsolute,
	dcnDrawFixedTextRelative,
	dcnDrawVariableTextAbsolute,
	dcnDrawVariableTextRelative,
	dcnDrawWideLineAbsolute32,
	dcnDrawCircleAbsolute32,
	dcnClearRectangle,
	dcnFillRectangle,
	dcnHighlightRectangle,
	dcnSetModeHighlight,
	dcnSetModeNormal,
	dcnSetModeXor,
	dcnSaveRectangle,
	dcnRestoreRectangle,
	dcnCopyRectangle,
	dcnKeybrdServe,
	dcnMouseServe,
	dcnVideoServe,
	dcnDisplayMessage,
	dcnEnd
};

#endif

/* -- END --------------------------------------------------------------- */
