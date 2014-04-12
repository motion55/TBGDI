/* ********************************************************************** */
/*                                                                        */
/*   DRIVER.H                                                             */
/*                                                                        */
/* ********************************************************************** */

#ifndef __DRIVER_H__
#define __DRIVER_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include "global.h"
#include "memory.h"
#include "resolutn.h"

/* -- PRAGMAS ----------------------------------------------------------- */

#pragma aux (DRIVER) DriverMain;
#pragma aux (DRIVER) InitDriver;
#pragma aux (DRIVER) ShutdownDriver;
#pragma aux (DRIVER) AllocBuffers;
#pragma aux (DRIVER) FreeBuffers;
#pragma aux (DRIVER) GetBitmapBufferAddress;
#pragma aux (DRIVER) PrintDriverInfo;
#pragma aux (DRIVER) GetScreenResolution;
#pragma aux (DRIVER) GetColor;
#pragma aux (DRIVER) SetColor;
#pragma aux (DRIVER) GetScale;
#pragma aux (DRIVER) SetScale;
#pragma aux (DRIVER) SetWindow;
#pragma aux (DRIVER) SetWindowOrigin;
#pragma aux (DRIVER) SetCursorState;
#pragma aux (DRIVER) SetCursorStyle;
#pragma aux (DRIVER) SaveWindowState;
#pragma aux (DRIVER) RestoreWindowState;
#pragma aux (DRIVER) DrawArcAbsolute;
#pragma aux (DRIVER) DrawWideArcAbsolute32;
#pragma aux (DRIVER) DrawWideLineAbsolute;
#pragma aux (DRIVER) DrawWideLineRelative;
#pragma aux (DRIVER) DrawCircleAbsolute;
#pragma aux (DRIVER) DrawCircleRelative;
#pragma aux (DRIVER) DrawDashedLineAbsolute;
#pragma aux (DRIVER) DrawDashedLineRelative;
#pragma aux (DRIVER) DrawGridDots;
#pragma aux (DRIVER) DrawGridDotsRelative;
#pragma aux (DRIVER) SetWindow32;
#pragma aux (DRIVER) DrawSolidLineAbsolute;
#pragma aux (DRIVER) DrawSolidLineRelative;
#pragma aux (DRIVER) MoveAbsolute;
#pragma aux (DRIVER) MoveRelative;
#pragma aux (DRIVER) DrawBitmapRelative;
#pragma aux (DRIVER) DrawFixedStringAbsolute;
#pragma aux (DRIVER) DrawFixedStringRelative;
#pragma aux (DRIVER) DrawVariableStringAbsolute;
#pragma aux (DRIVER) DrawVariableStringRelative;
#pragma aux (DRIVER) DrawFixedTextAbsolute;
#pragma aux (DRIVER) DrawFixedTextRelative;
#pragma aux (DRIVER) DrawVariableTextAbsolute;
#pragma aux (DRIVER) DrawVariableTextRelative;
#pragma aux (DRIVER) DrawWideLineAbsolute32;
#pragma aux (DRIVER) DrawCircleAbsolute32;
#pragma aux (DRIVER) ClearRectangle;
#pragma aux (DRIVER) FillRectangle;
#pragma aux (DRIVER) HighlightRectangle;
#pragma aux (DRIVER) SetModeHighlight;
#pragma aux (DRIVER) SetModeNormal;
#pragma aux (DRIVER) SetModeXor;
#pragma aux (DRIVER) SaveRectangle;
#pragma aux (DRIVER) RestoreRectangle;
#pragma aux (DRIVER) CopyRectangle;

/* -- TYPE DEFINITIONS -------------------------------------------------- */

struct DriverInfo {
	unsigned short Version;
	void far *Interface;
	unsigned short Magic;
	unsigned short Attributes;
};

/* -- EXTERNAL DATA DECLARATIONS ---------------------------------------- */

extern int end;

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern void PrintString(const char *s);

extern bool InitVideo(unsigned short resolutionX, unsigned short resolutionY);
extern void ShutdownVideo(void);
extern unsigned short DriverMain(struct DriverInfo far *const driverInfo);
extern void InitDriver(unsigned short far *const bitmapBufferSegment,
	unsigned short maxScale, unsigned char far *const dosError);
extern void ShutdownDriver(void);
extern void AllocBuffers(unsigned short far *const bitmapBufferSegment,
	unsigned short maxScale, unsigned char far *const dosError);
extern void FreeBuffers(void);
extern unsigned char far *GetBitmapBufferAddress();
extern void PrintDriverInfo();
extern void GetScreenResolution(unsigned short far *const maxX,
	unsigned short far *const maxY);
extern unsigned char GetColor();
extern void SetColor(unsigned char index);
extern unsigned short GetScale();
extern void SetScale(const unsigned short scale);
extern void SetWindow(short minX, short minY, short maxX, short maxY);
extern void SetWindowOrigin(unsigned short originX, unsigned short originY);
extern void SetCursorState(unsigned char state, short x, short y);
extern unsigned char SetCursorStyle(unsigned char newStyle);
extern void SaveWindowState();
extern void RestoreWindowState();
extern void DrawArcAbsolute(short xc, short yc, short xa, short ya,
	short xb, short yb, unsigned short radius, unsigned short thickness);
extern void DrawWideArcAbsolute32(long xc, long yc, long xa, long ya,
	long xb, long yb, unsigned long radius, unsigned long thickness,
	unsigned char fillMode);
extern void DrawWideLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawWideLineRelative(short dx, short dy);
extern void DrawCircleAbsolute(short xc, short yc, unsigned short length,
	unsigned short height, unsigned char fill);
extern void DrawCircleRelative(short dx, short dy, unsigned short length,
	unsigned short height, unsigned char fill);
extern void DrawDashedLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawDashedLineRelative(short dx, short dy);
extern void DrawGridDots(unsigned short spacing);
extern void DrawGridDotsRelative(unsigned short spacing, unsigned short dx,
	unsigned short dy);
extern void SetWindow32(long minX, long minY, long maxX, long maxY,
	unsigned long scale);
extern void DrawSolidLineAbsolute(short xa, short ya, short xb, short yb);
extern void DrawSolidLineRelative(short dx, short dy);
extern void MoveAbsolute(short x, short y);
extern void MoveRelative(short dx, short dy);
extern void DrawBitmapRelative(unsigned char far *const bitmap,
	unsigned short rows, unsigned short cols, unsigned short dx,
	unsigned short dy, unsigned short xsize, unsigned short ysize,
	unsigned char function);
extern void DrawFixedStringAbsolute(unsigned short length,
	const unsigned char far *string, short x, short y);
extern void DrawFixedStringRelative(unsigned short length,
	const unsigned char far *string, short dx, short dy);
extern void DrawVariableStringAbsolute(unsigned short length,
	const unsigned char far *string, short x, short y, short scale);
extern void DrawVariableStringRelative(unsigned short length,
	const unsigned char far *string, short dx, short dy, short scale);
extern void DrawFixedTextAbsolute(const unsigned char far *text, short x,
	short y);
extern void DrawFixedTextRelative(const unsigned char far *text, short dx,
	short dy);
extern void DrawVariableTextAbsolute(const unsigned char far *text,
	short x, short y, short scale);
extern void DrawVariableTextRelative(const unsigned char far *text,
	short dx, short dy, short scale);
extern void DrawWideLineAbsolute32(long xa, long ya, long xb, long yb,
	long thickness, unsigned char fillType);
extern void DrawCircleAbsolute32(long xc, long yc, long radius,
	unsigned char fill);
extern void ClearRectangle(short xa, short ya, short xb, short yb);
extern void FillRectangle(short xa, short ya, short xb, short yb);
extern void HighlightRectangle(short xa, short ya, short xb, short yb);
extern void SetModeHighlight();
extern void SetModeNormal();
extern void SetModeXor();
extern void SaveRectangle(short xa, short ya, short xb, short yb);
extern void RestoreRectangle();
extern void CopyRectangle(short xa, short ya, short xb, short yb);
extern unsigned short GetDriverLoadSize();

extern void DisplayMessage(char far *Message);

#endif

/* -- END --------------------------------------------------------------- */
