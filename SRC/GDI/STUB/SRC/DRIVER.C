/* ********************************************************************** */
/*                                                                        */
/*   DRIVER.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "driver.h"
#include <dos.h>
#include <dosfunc.h>

#define _DEBUG_ 0

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

unsigned char far *BitmapBuffer = NULL;
unsigned short far *ScaleLookupTable = NULL;

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static const void *DriverInterface[52] = {
	InitDriver,
	ShutdownDriver,
	AllocBuffers,
	FreeBuffers,
	GetBitmapBufferAddress,	//
	PrintDriverInfo,
	GetScreenResolution,
	GetColor,
	SetColor,
	GetScale,
	SetScale,
	SetWindow,
	SetWindowOrigin,
	SetCursorState,
	SetCursorStyle,
	SaveWindowState,
	RestoreWindowState,
	DrawArcAbsolute,
	DrawWideArcAbsolute32,
	DrawWideLineAbsolute,
	DrawWideLineRelative,
	DrawCircleAbsolute,
	DrawCircleRelative,
	DrawDashedLineAbsolute,
	DrawDashedLineRelative,
	DrawGridDots,
	DrawGridDotsRelative,
	SetWindow32,
	DrawSolidLineAbsolute,
	DrawSolidLineRelative,
	MoveAbsolute,
	MoveRelative,
	DrawBitmapRelative,
	DrawFixedStringAbsolute,
	DrawFixedStringRelative,
	DrawVariableStringAbsolute,
	DrawVariableStringRelative,
	DrawFixedTextAbsolute,
	DrawFixedTextRelative,
	DrawVariableTextAbsolute,
	DrawVariableTextRelative,
	DrawWideLineAbsolute32,
	DrawCircleAbsolute32,
	ClearRectangle,
	FillRectangle,
	HighlightRectangle,
	SetModeHighlight,
	SetModeNormal,
	SetModeXor,
	SaveRectangle,
	RestoreRectangle,
	CopyRectangle
};

char *FunctionNames[dcnEnd] = {
	"InitDriver\r\n",
	"ShutdownDriver\r\n",
	"PrintDriverInfo\r\n",
	"GetScreenResolution\r\n",
	"GetColor\r\n",
	"SetColor\r\n",
	"GetScale\r\n",
	"SetScale\r\n",
	"SetWindow\r\n",
	"SetWindowOrigin\r\n",
	"SetCursorState\r\n",
	"SetCursorStyle\r\n",
	"SaveWindowState\r\n",
	"RestoreWindowState\r\n",
	"DrawArcAbsolute\r\n",
	"DrawWideArcAbsolute32\r\n",
	"DrawWideLineAbsolute\r\n",
	"DrawWideLineRelative\r\n",
	"DrawCircleAbsolute\r\n",
	"DrawCircleRelative\r\n",
	"DrawDashedLineAbsolute\r\n",
	"DrawDashedLineRelative\r\n",
	"DrawGridDots\r\n",
	"DrawGridDotsRelative\r\n",
	"SetWindow32\r\n",
	"DrawSolidLineAbsolute\r\n",
	"DrawSolidLineRelative\r\n",
	"MoveAbsolute\r\n",
	"MoveRelative\r\n",
	"DrawBitmapRelative\r\n",
	"DrawFixedStringAbsolute\r\n",
	"DrawFixedStringRelative\r\n",
	"DrawVariableStringAbsolute\r\n",
	"DrawVariableStringRelative\r\n",
	"DrawFixedTextAbsolute\r\n",
	"DrawFixedTextRelative\r\n",
	"DrawVariableTextAbsolute\r\n",
	"DrawVariableTextRelative\r\n",
	"DrawWideLineAbsolute32\r\n",
	"DrawCircleAbsolute32\r\n",
	"ClearRectangle\r\n",
	"FillRectangle\r\n",
	"HighlightRectangle\r\n",
	"SetModeHighlight\r\n",
	"SetModeNormal\r\n",
	"SetModeXor\r\n",
	"SaveRectangle\r\n",
	"RestoreRectangle\r\n",
	"CopyRectangle\r\n"
	"KeybrdServe\r\n"
	"MouseServe\r\n"
	"VideoServe\r\n"
	"DisplayMessage\r\n"
};

static char CopyrightString[] =
	"TRaceBack's OrCAD Release IV/386+ Windows GDI Driver\r\n";

static unsigned short VDDHandle = NULL;
static bool VideoInitialized = false;

static union REGS in_regs;
static union REGS out_regs;
static struct SREGS sregs;

/* -- PRIVATE FUNCTION PROTOTYPES --------------------------------------- */

void interrupt *OrigKeybrdInt16 = NULL;
void interrupt KeybrdInt16(union INTPACK regs);

void interrupt *OrigMouseInt33 = NULL;
void interrupt MouseInt33(union INTPACK regs);

void interrupt *OrigTimerInt1C = NULL;
void interrupt TimerInt1C(union INTPACK regs);

void ResizeDriverMemoryBlock(unsigned short size);
void PrintChar(char ch);
void BIOSPrintChar(char ch);
void DOSPrintChar(char ch);
void KeepAwake(void);
unsigned short VDDRegisterModule(char far *name, char *init, char *dispatch,
	unsigned short *handle);
void VDDUnregisterModule(unsigned short handle);
unsigned short _VDDDispatchCall(unsigned short handle, unsigned short number,
	unsigned long far *arguments);
unsigned short VDDDispatchCall(unsigned short handle, unsigned short number,
	unsigned long far *arguments);

static void RedirectMouseKey(void);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   DriverMain                                                           */
/*                                                                        */
/* ====================================================================== */
unsigned short DriverMain(struct DriverInfo far *const driverInfo)
{
	unsigned short size = GetDriverLoadSize();

	if (driverInfo->Magic != 0xa55a) {
		PrintString(CopyrightString);
		ResizeDriverMemoryBlock(size);
	}

	driverInfo->Version = 13;
	driverInfo->Interface = DriverInterface;

	if ((driverInfo->Magic == 0xa55a) || (driverInfo->Magic == 0xa55b)) {
		driverInfo->Attributes = 0x10;
	}

	return size;
}

/* ====================================================================== */
/*                                                                        */
/*   InitDriver                                                           */
/*                                                                        */
/* ====================================================================== */
void InitDriver(unsigned short far *const bitmapBufferSegment,
	unsigned short maxScale, unsigned char far *const dosErrorCode)
{
	RedirectMouseKey();

	if (InitVideo(ResolutionX, ResolutionY) == false) {
		*dosErrorCode = -1;
		return;
	}

	AllocBuffers(bitmapBufferSegment, maxScale, dosErrorCode);
}

/* ====================================================================== */
/*                                                                        */
/*   ShutdownDriver                                                       */
/*                                                                        */
/* ====================================================================== */
void ShutdownDriver()
{
	if (OrigKeybrdInt16!=NULL)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x16;
		in_regs.w.dx = FP_OFF(OrigKeybrdInt16);
		sregs.ds     = FP_SEG(OrigKeybrdInt16);
		intdosx(&in_regs, &out_regs, &sregs);

		OrigKeybrdInt16 = NULL;
	}

	if (OrigMouseInt33!=NULL)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x33;
		in_regs.w.dx = FP_OFF(OrigMouseInt33);
		sregs.ds     = FP_SEG(OrigMouseInt33);
		intdosx(&in_regs, &out_regs, &sregs);

		OrigMouseInt33 = NULL;
	}

	if (OrigTimerInt1C!=NULL)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x1C;
		in_regs.w.dx = FP_OFF(OrigTimerInt1C);
		sregs.ds     = FP_SEG(OrigTimerInt1C);
		intdosx(&in_regs, &out_regs, &sregs);

		OrigTimerInt1C = NULL;
	}

	FreeBuffers();
	ShutdownVideo();
}

/* ====================================================================== */
/*                                                                        */
/*   AllocBuffers                                                         */
/*                                                                        */
/* ====================================================================== */
void AllocBuffers(unsigned short far *const bitmapBufferSegment,
	unsigned short maxScale, unsigned char far *const dosErrorCode)
{
	if (BitmapBuffer == NULL) {
		*dosErrorCode = AllocMemory((16384 / 16),
			(void far* far*)&BitmapBuffer);
		if (*dosErrorCode != 0) {
			FreeBuffers();
			return;
		}
	}

	if ((ScaleLookupTable == NULL) && (maxScale > 1)) {
		*dosErrorCode = AllocMemory((65536 / 16),
			(void far* far*)&ScaleLookupTable);
		if (*dosErrorCode != 0) {
			FreeBuffers();
			return;
		}
	}

	*dosErrorCode = 0;
	*bitmapBufferSegment = FP_SEG(BitmapBuffer);
}

/* ====================================================================== */
/*                                                                        */
/*   FreeBuffers                                                          */
/*                                                                        */
/* ====================================================================== */
void FreeBuffers()
{
	if (BitmapBuffer != NULL) {
		FreeMemory(BitmapBuffer);
		BitmapBuffer = NULL;
	}

	if (ScaleLookupTable != NULL) {
		FreeMemory(ScaleLookupTable);
		ScaleLookupTable = NULL;
	}
}

/* ====================================================================== */
/*                                                                        */
/*   InitVideo                                                            */
/*                                                                        */
/* ====================================================================== */
bool InitVideo(unsigned short resolutionX, unsigned short resolutionY)
{
	unsigned long arguments[2];

	if (VideoInitialized == true) {
		return true;
	}

	arguments[0] = (unsigned long)resolutionX;
	arguments[1] = (unsigned long)resolutionY;

	VDDDispatchCall(VDDHandle, dcnInitDriver, &arguments);

	if (resolutionX==0)
		ResolutionX = (unsigned short)arguments[0];
	if (resolutionY==0)
		ResolutionY = (unsigned short)arguments[1];

	VideoInitialized = true;
	return true;
}

/* ====================================================================== */
/*                                                                        */
/*   ShutdownVideo                                                        */
/*                                                                        */
/* ====================================================================== */
void ShutdownVideo()
{
	if (VideoInitialized == false) {
		return;
	}
	VDDDispatchCall(VDDHandle, dcnShutdownDriver, NULL);
	VDDUnregisterModule(VDDHandle);
	VideoInitialized = false;
	VDDHandle = NULL;
}

/* ====================================================================== */
/*                                                                        */
/*   GetBitmapBufferAddress                                               */
/*                                                                        */
/* ====================================================================== */
unsigned char far *GetBitmapBufferAddress()
{
	return BitmapBuffer;
}

/* ====================================================================== */
/*                                                                        */
/*   PrintDriverInfo                                                      */
/*                                                                        */
/* ====================================================================== */
void PrintDriverInfo()
{
	VDDDispatchCall(VDDHandle, dcnPrintDriverInfo, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   GetScreenResolution                                                  */
/*                                                                        */
/* ====================================================================== */
void GetScreenResolution(unsigned short far *const maxX,
	unsigned short far *const maxY)
{
	unsigned long arguments[2];

	RedirectMouseKey();

#if _DEBUG_
	PrintString("GetScreenResolution\r\n");
#endif

	if ((ResolutionX==0)&&(ResolutionY==0))
	{
		arguments[0] = 0;
		arguments[1] = 0;

		VDDDispatchCall(VDDHandle, dcnGetScreenResolution, &arguments);

		*maxX = arguments[0];
		*maxY = arguments[1];
	}
	else
	{
		*maxX = ResolutionX - 1;
		*maxY = ResolutionY - 1;
	}
}

/* ====================================================================== */
/*                                                                        */
/*   GetColor                                                             */
/*                                                                        */
/* ====================================================================== */
unsigned char GetColor()
{
	return (unsigned char)VDDDispatchCall(VDDHandle, dcnGetColor, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   SetColor                                                             */
/*                                                                        */
/* ====================================================================== */
void SetColor(unsigned char index)
{
	unsigned long arguments[1];

	arguments[0] = (unsigned long)index;

	VDDDispatchCall(VDDHandle, dcnSetColor, &arguments);
}
/* ====================================================================== */
/*                                                                        */
/*   GetScale                                                             */
/*                                                                        */
/* ====================================================================== */
unsigned short GetScale()
{
	return VDDDispatchCall(VDDHandle, dcnGetScale, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   SetScale                                                             */
/*                                                                        */
/* ====================================================================== */
void SetScale(unsigned short scale)
{
	unsigned long arguments[1];

	arguments[0] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnSetScale, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetWindow                                                            */
/*                                                                        */
/* ====================================================================== */
void SetWindow(short minX, short minY, short maxX, short maxY)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)minX;
	arguments[1] = (unsigned long)minY;
	arguments[2] = (unsigned long)maxX;
	arguments[3] = (unsigned long)maxY;

	VDDDispatchCall(VDDHandle, dcnSetWindow, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetWindowOrigin                                                      */
/*                                                                        */
/* ====================================================================== */
void SetWindowOrigin(unsigned short originX, unsigned short originY)
{
	unsigned long arguments[2];

	arguments[0] = (unsigned long)originX;
	arguments[1] = (unsigned long)originY;

	VDDDispatchCall(VDDHandle, dcnSetWindowOrigin, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetCursorState                                                       */
/*                                                                        */
/* ====================================================================== */
void SetCursorState(unsigned char state, short x, short y)
{
	unsigned long arguments[3];

	arguments[0] = (unsigned long)state;
	arguments[1] = (unsigned long)x;
	arguments[2] = (unsigned long)y;

	VDDDispatchCall(VDDHandle, dcnSetCursorState, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetCursorStyle                                                       */
/*                                                                        */
/* ====================================================================== */
unsigned char SetCursorStyle(unsigned char newStyle)
{
	unsigned long arguments[1];

	arguments[0] = (unsigned long)newStyle;

	return (unsigned char)VDDDispatchCall(VDDHandle, dcnSetCursorStyle,
		&arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SaveWindowState                                                      */
/*                                                                        */
/* ====================================================================== */
void SaveWindowState()
{
	VDDDispatchCall(VDDHandle, dcnSaveWindowState, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   RestoreWindowState                                                   */
/*                                                                        */
/* ====================================================================== */
void RestoreWindowState()
{
	VDDDispatchCall(VDDHandle, dcnRestoreWindowState, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawArcAbsolute                                                      */
/*                                                                        */
/* ====================================================================== */
void DrawArcAbsolute(short xc, short yc, short xa, short ya,
	short xb, short yb, unsigned short radius, unsigned short thickness)
{
	unsigned long arguments[8];

	arguments[0] = (unsigned long)xc;
	arguments[1] = (unsigned long)yc;
	arguments[2] = (unsigned long)xa;
	arguments[3] = (unsigned long)ya;
	arguments[4] = (unsigned long)xb;
	arguments[5] = (unsigned long)yb;
	arguments[6] = (unsigned long)radius;
	arguments[7] = (unsigned long)thickness;

	VDDDispatchCall(VDDHandle, dcnDrawArcAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideArcAbsolute32                                                */
/*                                                                        */
/* ====================================================================== */
void DrawWideArcAbsolute32(long xc, long yc, long xa, long ya,
	long xb, long yb, unsigned long radius, unsigned long thickness,
	unsigned char fillMode)
{
	unsigned long arguments[9];

	arguments[0] = (unsigned long)xc;
	arguments[1] = (unsigned long)yc;
	arguments[2] = (unsigned long)xa;
	arguments[3] = (unsigned long)ya;
	arguments[4] = (unsigned long)xb;
	arguments[5] = (unsigned long)yb;
	arguments[6] = (unsigned long)radius;
	arguments[7] = (unsigned long)thickness;
	arguments[8] = (unsigned long)fillMode;

	VDDDispatchCall(VDDHandle, dcnDrawWideArcAbsolute32, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineAbsolute                                                 */
/*                                                                        */
/* ====================================================================== */
void DrawWideLineAbsolute(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnDrawWideLineAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineRelative                                                 */
/*                                                                        */
/* ====================================================================== */
void DrawWideLineRelative(short dx, short dy)
{
	unsigned long arguments[2];

	arguments[0] = (unsigned long)dx;
	arguments[1] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawWideLineRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleAbsolute                                                   */
/*                                                                        */
/* ====================================================================== */
void DrawCircleAbsolute(short xc, short yc, unsigned short length,
	unsigned short height, unsigned char fill)
{
	unsigned long arguments[5];

	arguments[0] = (unsigned long)xc;
	arguments[1] = (unsigned long)yc;
	arguments[2] = (unsigned long)length;
	arguments[3] = (unsigned long)height;
	arguments[4] = (unsigned long)fill;

	VDDDispatchCall(VDDHandle, dcnDrawCircleAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleRelative                                                   */
/*                                                                        */
/* ====================================================================== */
void DrawCircleRelative(short dx, short dy, unsigned short length,
	unsigned short height, unsigned char fill)
{
	unsigned long arguments[5];

	arguments[0] = (unsigned long)dx;
	arguments[1] = (unsigned long)dy;
	arguments[2] = (unsigned long)length;
	arguments[3] = (unsigned long)height;
	arguments[4] = (unsigned long)fill;

	VDDDispatchCall(VDDHandle, dcnDrawCircleRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawDashedLineAbsolute                                               */
/*                                                                        */
/* ====================================================================== */
void DrawDashedLineAbsolute(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnDrawDashedLineAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawDashedLineRelative                                               */
/*                                                                        */
/* ====================================================================== */
void DrawDashedLineRelative(short dx, short dy)
{
	unsigned long arguments[2];

	arguments[0] = (unsigned long)dx;
	arguments[1] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawDashedLineRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawGridDots                                                         */
/*                                                                        */
/* ====================================================================== */
void DrawGridDots(unsigned short spacing)
{
	unsigned long arguments[1];

	arguments[0] = (unsigned long)spacing;

	VDDDispatchCall(VDDHandle, dcnDrawGridDots, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawGridDotsRelative                                                 */
/*                                                                        */
/* ====================================================================== */
void DrawGridDotsRelative(unsigned short spacing, unsigned short dx,
	unsigned short dy)
{
	unsigned long arguments[3];

	arguments[0] = (unsigned long)spacing;
	arguments[1] = (unsigned long)dx;
	arguments[2] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawGridDotsRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetWindow32	                                                  */
/*                                                                        */
/* ====================================================================== */
void SetWindow32(long minX, long minY, long maxX, long maxY,
	unsigned long scale)
{
	unsigned long arguments[5];

	arguments[0] = (unsigned long)minX;
	arguments[1] = (unsigned long)minY;
	arguments[2] = (unsigned long)maxX;
	arguments[3] = (unsigned long)maxY;
	arguments[4] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnSetWindow32, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawSolidLineAbsolute                                                */
/*                                                                        */
/* ====================================================================== */
void DrawSolidLineAbsolute(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnDrawSolidLineAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawSolidLineRelative                                                */
/*                                                                        */
/* ====================================================================== */
void DrawSolidLineRelative(short dx, short dy)
{
	unsigned long arguments[2];

	arguments[0] = (unsigned long)dx;
	arguments[1] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawSolidLineRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   MoveAbsolute                                                         */
/*                                                                        */
/* ====================================================================== */
void MoveAbsolute(short x, short y)
{
	unsigned long arguments[2];

	arguments[0] = (unsigned long)x;
	arguments[1] = (unsigned long)y;

	VDDDispatchCall(VDDHandle, dcnMoveAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   MoveRelative                                                         */
/*                                                                        */
/* ====================================================================== */
void MoveRelative(short dx, short dy)
{
	unsigned long arguments[2];

	arguments[0] = dx;
	arguments[1] = dy;

	VDDDispatchCall(VDDHandle, dcnMoveRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawBitmapRelative                                                   */
/*                                                                        */
/* ====================================================================== */
void DrawBitmapRelative(unsigned char far *const bitmap, unsigned short rows,
	unsigned short cols, unsigned short dx, unsigned short dy,
	unsigned short xsize, unsigned short ysize, unsigned char function)
{
	unsigned long arguments[9];

	arguments[0] = FP_SEG(bitmap);
	arguments[1] = FP_OFF(bitmap);
	arguments[2] = (unsigned long)rows;
	arguments[3] = (unsigned long)cols;
	arguments[4] = (unsigned long)dx;
	arguments[5] = (unsigned long)dy;
	arguments[6] = (unsigned long)xsize;
	arguments[7] = (unsigned long)ysize;
	arguments[8] = (unsigned long)function;

	VDDDispatchCall(VDDHandle, dcnDrawBitmapRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedStringAbsolute                                              */
/*                                                                        */
/* ====================================================================== */
void DrawFixedStringAbsolute(unsigned short length,
	const unsigned char far *string, short x, short y)
{
	unsigned long arguments[5];

	arguments[0] = (unsigned long)length;
	arguments[1] = FP_SEG(string);
	arguments[2] = FP_OFF(string);
	arguments[3] = (unsigned long)x;
	arguments[4] = (unsigned long)y;

	VDDDispatchCall(VDDHandle, dcnDrawFixedStringAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedStringRelative                                              */
/*                                                                        */
/* ====================================================================== */
void DrawFixedStringRelative(unsigned short length,
	const unsigned char far *string, short dx, short dy)
{
	unsigned long arguments[5];

	arguments[0] = (unsigned long)length;
	arguments[1] = FP_SEG(string);
	arguments[2] = FP_OFF(string);
	arguments[3] = (unsigned long)dx;
	arguments[4] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawFixedStringRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableStringAbsolute                                           */
/*                                                                        */
/* ====================================================================== */
void DrawVariableStringAbsolute(unsigned short length,
	const unsigned char far *string, short x, short y, short scale)
{
	unsigned long arguments[6];

	arguments[0] = (unsigned long)length;
	arguments[1] = FP_SEG(string);
	arguments[2] = FP_OFF(string);
	arguments[3] = (unsigned long)x;
	arguments[4] = (unsigned long)y;
	arguments[5] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnDrawVariableStringAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableStringRelative                                           */
/*                                                                        */
/* ====================================================================== */
void DrawVariableStringRelative(unsigned short length,
	const unsigned char far *string, short dx, short dy, short scale)
{
	unsigned long arguments[6];

	arguments[0] = (unsigned long)length;
	arguments[1] = FP_SEG(string);
	arguments[2] = FP_OFF(string);
	arguments[3] = (unsigned long)dx;
	arguments[4] = (unsigned long)dy;
	arguments[5] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnDrawVariableStringRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedTextAbsolute                                                */
/*                                                                        */
/* ====================================================================== */
void DrawFixedTextAbsolute(const unsigned char far *text, short x, short y)
{
	unsigned long arguments[4];

	arguments[0] = FP_SEG(text);
	arguments[1] = FP_OFF(text);
	arguments[2] = (unsigned long)x;
	arguments[3] = (unsigned long)y;

	VDDDispatchCall(VDDHandle, dcnDrawFixedTextAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedTextRelative                                                */
/*                                                                        */
/* ====================================================================== */
void DrawFixedTextRelative(const unsigned char far *text, short dx, short dy)
{
	unsigned long arguments[4];

	arguments[0] = FP_SEG(text);
	arguments[1] = FP_OFF(text);
	arguments[2] = (unsigned long)dx;
	arguments[3] = (unsigned long)dy;

	VDDDispatchCall(VDDHandle, dcnDrawFixedTextRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableTextAbsolute                                             */
/*                                                                        */
/* ====================================================================== */
void DrawVariableTextAbsolute(const unsigned char far *text, short x,
	short y, short scale)
{
	unsigned long arguments[5];

	arguments[0] = FP_SEG(text);
	arguments[1] = FP_OFF(text);
	arguments[2] = (unsigned long)x;
	arguments[3] = (unsigned long)y;
	arguments[4] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnDrawVariableTextAbsolute, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableTextRelative                                             */
/*                                                                        */
/* ====================================================================== */
void DrawVariableTextRelative(const unsigned char far *text,
	short dx, short dy, short scale)
{
	unsigned long arguments[5];

	arguments[0] = FP_SEG(text);
	arguments[1] = FP_OFF(text);
	arguments[2] = (unsigned long)dx;
	arguments[3] = (unsigned long)dy;
	arguments[4] = (unsigned long)scale;

	VDDDispatchCall(VDDHandle, dcnDrawVariableTextRelative, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawWideLineAbsolute32                                               */
/*                                                                        */
/* ====================================================================== */
void DrawWideLineAbsolute32(long xa, long ya, long xb, long yb,
	long thickness, unsigned char fillType)
{
	unsigned long arguments[6];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;
	arguments[4] = (unsigned long)thickness;
	arguments[5] = (unsigned long)fillType;

	VDDDispatchCall(VDDHandle, dcnDrawWideLineAbsolute32, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCircleAbsolute32                                                 */
/*                                                                        */
/* ====================================================================== */
void DrawCircleAbsolute32(long xc, long yc, long radius, unsigned char fill)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xc;
	arguments[1] = (unsigned long)yc;
	arguments[2] = (unsigned long)radius;
	arguments[3] = (unsigned long)fill;

	VDDDispatchCall(VDDHandle, dcnDrawCircleAbsolute32, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   ClearRectangle                                                       */
/*                                                                        */
/* ====================================================================== */
void ClearRectangle(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnClearRectangle, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   FillRectangle                                                        */
/*                                                                        */
/* ====================================================================== */
void FillRectangle(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnFillRectangle, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   HighlightRectangle                                                   */
/*                                                                        */
/* ====================================================================== */
void HighlightRectangle(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnHighlightRectangle, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   SetModeHighlight                                                     */
/*                                                                        */
/* ====================================================================== */
void SetModeHighlight()
{
	VDDDispatchCall(VDDHandle, dcnSetModeHighlight, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   SetModeNormal                                                        */
/*                                                                        */
/* ====================================================================== */
void SetModeNormal()
{
	VDDDispatchCall(VDDHandle, dcnSetModeNormal, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   SetModeXor                                                           */
/*                                                                        */
/* ====================================================================== */

void SetModeXor()
{
	VDDDispatchCall(VDDHandle, dcnSetModeXor, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   SaveRectangle                                                        */
/*                                                                        */
/* ====================================================================== */
void SaveRectangle(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnSaveRectangle, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   RestoreRectangle                                                     */
/*                                                                        */
/* ====================================================================== */
void RestoreRectangle()
{
	VDDDispatchCall(VDDHandle, dcnRestoreRectangle, NULL);
}

/* ====================================================================== */
/*                                                                        */
/*   CopyRectangle                                                        */
/*                                                                        */
/* ====================================================================== */
void CopyRectangle(short xa, short ya, short xb, short yb)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)xa;
	arguments[1] = (unsigned long)ya;
	arguments[2] = (unsigned long)xb;
	arguments[3] = (unsigned long)yb;

	VDDDispatchCall(VDDHandle, dcnCopyRectangle, &arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   GetDriverLoadSize                                                    */
/*                                                                        */
/* ====================================================================== */
unsigned short GetDriverLoadSize()
{
	return (((unsigned short)&end + 15) / 16);
}

/* ====================================================================== */
/*                                                                        */
/*   PrintString                                                          */
/*                                                                        */
/* ====================================================================== */
void PrintString(const char *s)
{
	for (; *s; s++) {
#if 1
		BIOSPrintChar(*s);
#else
		DOSPrintChar(*s);
#endif
	}
}

/* ====================================================================== */
/*                                                                        */
/*   ResizeDriverMemoryBlock                                              */
/*                                                                        */
/* ====================================================================== */
#pragma aux ResizeDriverMemoryBlock =	\
	"mov	ah, 4ah"		\
	"push	cs"			\
	"pop	es"			\
	"int	21h"			\
	parm [bx] nomemory		\
	modify exact [ax bx es] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   DOSPrintChar                                                         */
/*                                                                        */
/* ====================================================================== */
#pragma aux DOSPrintChar =		\
	"mov	ah, 02h"		\
	"int	21h"			\
	parm [dl] nomemory		\
	modify exact [ax] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   BIOSPrintChar                                                        */
/*                                                                        */
/* ====================================================================== */
#pragma aux BIOSPrintChar =	\
	"mov	ah, 0Eh"		\
	"xor	bx,bx"			\
	"int	10h"			\
	parm [al] nomemory		\
	modify exact [ax bx] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   KeepAwake                                                            */
/*                                                                        */
/* ====================================================================== */
#pragma aux KeepAwake =	\
	"pushf"	          	\
	"call	dword ptr OrigTimerInt1C"	\
	"mov	ah, 0Fh" 	\
	"int	10h"        \
	modify exact [ax bx] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   VDDRegisterModule                                                    */
/*                                                                        */
/* ====================================================================== */

#pragma aux VDDRegisterModule =	\
	"db	0c4h"				\
	"db	0c4h"				\
	"db	58h"				\
	"db	00h"				\
	"jc	error"				\
	"mov	bx, dx"				\
	"mov	ds:[bx], ax"		\
	"xor	ax, ax"				\
	"error:"			    	\
	parm [es si] [di] [bx] [dx]	\
	value [ax]	              	\
	modify exact [ax bx dx si di es] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   VDDUnregisterModule                                                  */
/*                                                                        */
/* ====================================================================== */
#pragma aux VDDUnregisterModule =		\
	"db	0c4h"				\
	"db	0c4h"				\
	"db	58h"				\
	"db	01h"				\
	parm [ax]				\
	modify exact [ax] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   VDDDispatchCall                                                      */
/*                                                                        */
/* ====================================================================== */
unsigned short VDDDispatchCall(unsigned short handle, unsigned short number,
	unsigned long far *arguments)
{
	unsigned short vddResult;

#if _DEBUG_
	if (number<dcnKeybrdServe)
		PrintString(FunctionNames[number]);
#endif

	if (VDDHandle==NULL)
	{
		vddResult = VDDRegisterModule("tbgdi.dll", "_VDDInit@0",
			"_VDDDispatchCall@0", &VDDHandle);

		if (vddResult != 0)
		{
			switch (vddResult) {
			case 1:
				PrintString("TBGDI.DLL not found!\r\n");
				break;
			case 2:
				PrintString("Dispatch routine not found!\r\n");
				break;
			case 3:
				PrintString("Init routine not found!\r\n");
				break;
			case 4:
				PrintString("VDD Insufficient memory!\r\n");
				break;
			default:
				PrintString("Unknown error!\r\n");
				break;
			}
			VDDHandle = NULL;
			return 0;
		}
	}

	return _VDDDispatchCall(VDDHandle, number, arguments);
}

/* ====================================================================== */
/*                                                                        */
/*   _VDDDispatchCall                                                     */
/*                                                                        */
/* ====================================================================== */
#pragma aux _VDDDispatchCall =			\
	"push	ds"				\
	"push	es"				\
	"db	0c4h"				\
	"db	0c4h"				\
	"db	58h"				\
	"db	02h"				\
	"pop	es"				\
	"pop	ds"				\
	parm [ax] [bx] [es di]	\
	value [ax]          	\
	modify exact [ax bx es di] nomemory;

/* ====================================================================== */
/*                                                                        */
/*   DisplayMessage                                                       */
/*                                                                        */
/* ====================================================================== */
void DisplayMessage(char far *Message)
{
	VDDDispatchCall(VDDHandle, dcnDisplayMessage,
		(unsigned long far *)Message);
}

/* ====================================================================== */
/*                                                                        */
/*   Keyboard Int 16h service routine                                     */
/*                                                                        */
/* ====================================================================== */
void interrupt KeybrdInt16(union INTPACK regs)
{
	unsigned long arguments[3];

	arguments[0] = (unsigned long)regs.w.ax;
	arguments[1] = (unsigned long)regs.w.bx;
	arguments[2] = (unsigned long)regs.w.flags;

	VDDDispatchCall(VDDHandle, dcnKeybrdServe, &arguments);

	regs.w.ax = (unsigned short)arguments[0];
	regs.w.bx = (unsigned short)arguments[1];
	regs.w.flags = (unsigned)arguments[2];
}

/* ====================================================================== */
/*                                                                        */
/*   Mouse Int 33h service routine                                        */
/*                                                                        */
/* ====================================================================== */
void interrupt MouseInt33(union INTPACK regs)
{
	unsigned long arguments[4];

	arguments[0] = (unsigned long)regs.w.ax;
	arguments[1] = (unsigned long)regs.w.bx;
	arguments[2] = (unsigned long)regs.w.cx;
	arguments[3] = (unsigned long)regs.w.dx;

	VDDDispatchCall(VDDHandle, dcnMouseServe, &arguments);

	regs.w.ax = (unsigned short)arguments[0];
	regs.w.bx = (unsigned short)arguments[1];
	regs.w.cx = (unsigned short)arguments[2];
	regs.w.dx = (unsigned short)arguments[3];
}

/* ====================================================================== */
/*                                                                        */
/*   Video Int 10h service routine                                        */
/*                                                                        */
/* ====================================================================== */
void interrupt VideoInt10(union INTPACK regs)
{
	unsigned long arguments[4];

	arguments[0] = regs.w.ax;
	arguments[1] = regs.w.bx;
	arguments[2] = regs.w.cx;
	arguments[3] = regs.w.dx;
	arguments[4] = regs.w.bp;
	arguments[5] = regs.w.es;

	VDDDispatchCall(VDDHandle, dcnVideoServe, &arguments);

	regs.w.ax = (unsigned short)arguments[0];
	regs.w.bx = (unsigned short)arguments[1];
	regs.w.cx = (unsigned short)arguments[2];
	regs.w.dx = (unsigned short)arguments[3];
	regs.w.bp = (unsigned short)arguments[4];
	regs.w.es = (unsigned short)arguments[5];
}

/* ====================================================================== */
/*                                                                        */
/*   Timer Int 1Ch service routine                                        */
/*                                                                        */
/* ====================================================================== */
void interrupt TimerInt1C(union INTPACK regs)
{
	KeepAwake();
}

/* ====================================================================== */
/*                                                                        */
/*   RedirectMouseKey                                                     */
/*                                                                        */
/* ====================================================================== */
void RedirectMouseKey(void)
{
	void interrupt *vector;

	in_regs.h.ah = DOS_GET_INT;
	in_regs.h.al = 0x16;
	intdosx(&in_regs, &out_regs, &sregs);
	vector = MK_FP(sregs.es,out_regs.w.bx);
	if (OrigKeybrdInt16==NULL)
	{
		OrigKeybrdInt16 = vector;
	}

	if (vector != &KeybrdInt16)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x16;
		in_regs.w.dx = FP_OFF(&KeybrdInt16);
		sregs.ds     = FP_SEG(&KeybrdInt16);
		intdosx(&in_regs, &out_regs, &sregs);
	}

	in_regs.h.ah = DOS_GET_INT;
	in_regs.h.al = 0x33;
	intdosx(&in_regs, &out_regs, &sregs);
	vector = MK_FP(sregs.es,out_regs.w.bx);
	if (OrigMouseInt33==NULL)
	{
		OrigMouseInt33 = vector;
	}

	if (vector != &MouseInt33)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x33;
		in_regs.w.dx = FP_OFF(&MouseInt33);
		sregs.ds     = FP_SEG(&MouseInt33);
		intdosx(&in_regs, &out_regs, &sregs);
	}


	in_regs.h.ah = DOS_GET_INT;
	in_regs.h.al = 0x1C;
	intdosx(&in_regs, &out_regs, &sregs);
	vector = MK_FP(sregs.es,out_regs.w.bx);
	if (OrigTimerInt1C==NULL)
	{
		OrigTimerInt1C = vector;
	}

#if _DEBUG_
#else
	if (vector != &TimerInt1C)
	{
		in_regs.h.ah = DOS_SET_INT;
		in_regs.h.al = 0x1C;
		in_regs.w.dx = FP_OFF(&TimerInt1C);
		sregs.ds     = FP_SEG(&TimerInt1C);
		intdosx(&in_regs, &out_regs, &sregs);
	}
#endif
}

/* -- END --------------------------------------------------------------- */
