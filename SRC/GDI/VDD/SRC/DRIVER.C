/* ********************************************************************** */
/*                                                                        */
/*   DRIVER.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "driver.h"
#include <stdio.h>

#define USE_MESSAGE_BOX	0

#if USE_MESSAGE_BOX
char mess[40];
#endif

#define MIN_RESX    1600
#define MIN_RESY    1200

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

HINSTANCE hInstance;
int IdleCounter = DEF_IDLECOUNT;

static void InitDriver(void);
static void ShutdownDriver(void);
static void WGetScreenResolution(void);
static void WDrawWideLineAbsolute(void);
static void WDrawWideLineRelative(void);
static void WDrawSolidLineAbsolute(void);
static void WDrawSolidLineRelative(void);
static void WDrawDashedLineAbsolute(void);
static void WDrawDashedLineRelative(void);
static void WGetColor(void);
static void WSetColor(void);
static void WSetCursorStyle(void);
static void WSetCursorState(void);
static void WGetScale(void);
static void WSetScale(void);
static void WSetWindow(void);
static void WSetWindowOrigin(void);
static void WMoveAbsolute(void);
static void WMoveRelative(void);
static void WClearRectangle(void);
static void WFillRectangle(void);
static void WHighlightRectangle(void);
static void WDrawArcAbsolute(void);
static void WDrawWideArcAbsolute32(void);
static void WDrawCircleAbsolute(void);
static void WDrawCircleRelative(void);
static void WDrawCircleAbsolute32(void);
static void WDrawGridDots(void);
static void WDrawGridDotsRelative(void);
static void WSetWindow32(void);
static void WDrawBitmapRelative(void);
static void WDrawWideLineAbsolute32(void);
static void WDrawFixedStringAbsolute(void);
static void WDrawFixedStringRelative(void);
static void WDrawVariableStringAbsolute(void);
static void WDrawVariableStringRelative(void);
static void WDrawFixedTextAbsolute(void);
static void WDrawFixedTextRelative(void);
static void WDrawVariableTextAbsolute(void);
static void WDrawVariableTextRelative(void);
static void WSaveRectangle(void);
static void WCopyRectangle(void);

static void WKeybrdServe(void);
static void WMouseServe(void);
static void WVideoServe(void);
static void WDisplayMessage(void);

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static void *DriverInterface[dcnEnd] = {
	InitDriver,
	ShutdownDriver,
	PrintDriverInfo,	/* PrintDriverInfo */
	WGetScreenResolution,	/* GetScreenResolution */
	WGetColor,			/* GetColor */
	WSetColor,			/* SetColor */
	WGetScale,			/* GetScale */
	WSetScale,			/* SetScale */
	WSetWindow,			/* SetWindow */
	WSetWindowOrigin,		/* SetWindowOrigin */
	WSetCursorState,		/* SetCursorState */
	WSetCursorStyle,		/* SetCursorStyle */
	SaveWindowState,		/* SaveWindowState */
	RestoreWindowState,		/* RestoreWindowState */
	WDrawArcAbsolute,		/* DrawArcAbsolute */
	WDrawWideArcAbsolute32,		/* DrawWideArcAbsolute32 */
	WDrawWideLineAbsolute,		/* DrawWideLineAbsolute */
	WDrawWideLineRelative,		/* DrawWideLineRelative */
	WDrawCircleAbsolute,		/* DrawCircleAbsolute */
	WDrawCircleRelative,		/* DrawCircleRelative */
	WDrawDashedLineAbsolute,	/* DrawDashedLineAbsolute */
	WDrawDashedLineRelative,	/* DrawDashedLineRelative */
	WDrawGridDots,				/* DrawGridDots */
	WDrawGridDotsRelative,		/* DrawGridDotsRelative */
	WSetWindow32,			/* SetWindow32 */
	WDrawSolidLineAbsolute,		/* DrawSolidLineAbsolute */
	WDrawSolidLineRelative,		/* DrawSolidLineRelative */
	WMoveAbsolute,			/* MoveAbsolute */
	WMoveRelative,			/* MoveRelative */
	WDrawBitmapRelative,		/* DrawBitmapRelative */
	WDrawFixedStringAbsolute,	/* DrawFixedStringAbsolute */
	WDrawFixedStringRelative,	/* DrawFixedStringRelative */
	WDrawVariableStringAbsolute,	/* DrawVariableStringAbsolute */
	WDrawVariableStringRelative,	/* DrawVariableStringRelative */
	WDrawFixedTextAbsolute,		/* DrawFixedTextAbsolute */
	WDrawFixedTextRelative,		/* DrawFixedTextRelative */
	WDrawVariableTextAbsolute,	/* DrawVariableTextAbsolute */
	WDrawVariableTextRelative,	/* DrawVariableTextRelative */
	WDrawWideLineAbsolute32,	/* DrawWideLineAbsolute32 */
	WDrawCircleAbsolute32,		/* DrawCircleAbsolute32 */
	WClearRectangle,		/* ClearRectangle */
	WFillRectangle,			/* FillRectangle */
	WHighlightRectangle,		/* HighlightRectangle */
	SetModeHighlight,		/* SetModeHighlight */
	SetModeNormal,			/* SetModeNormal */
	SetModeXor,			/* SetModeXor */
	WSaveRectangle,		/* SaveRectangle */
	RestoreRectangle,	/* RestoreRectangle */
	WCopyRectangle,		/* CopyRectangle */
	WKeybrdServe,
	WMouseServe,
	WVideoServe,
	WDisplayMessage
};

/* -- PRIVATE FUNCTION PROTOTYPES --------------------------------------- */

__declspec(dllexport) void CALLBACK VDDInit(void);
__declspec(dllexport) void CALLBACK VDDDispatchCall(void);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   DllMain                                                              */
/*                                                                        */
/* ====================================================================== */

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			hInstance = hInstDLL;
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		default:
			break;
	}

	return TRUE;
}

/* ====================================================================== */
/*                                                                        */
/*   VDDInit                                                              */
/*                                                                        */
/* ====================================================================== */

void WINAPI VDDInit()
{
#if USE_MESSAGE_BOX
	MessageBox(NULL, NULL, "VDDInit", MB_OK);
#endif
	setCF(0);
}

/* ====================================================================== */
/*                                                                        */
/*   VDDDispatchCall                                                      */
/*                                                                        */
/* ====================================================================== */

void WINAPI VDDDispatchCall()
{
	unsigned short dcn;
	void (*function)(void);

	dcn = getBX();
#ifdef	USE_UNIFIED_MUTEX
	if (dcn >= dcnDrawArcAbsolute && dcn <= dcnCopyRectangle)
		WaitForSingleObject(hmMemoryBuffer, INFINITE);
#endif
	if (dcn < dcnEnd) {
		function = DriverInterface[dcn];
		if (function != NULL) {
			function();
			if (dcn<dcnKeybrdServe)
			{
				IdleCounter = DEF_IDLECOUNT;
				SetEvent(hKeyMouseEvent);
			}
		}
	}
#ifdef	USE_UNIFIED_MUTEX
	if (dcn >= dcnDrawArcAbsolute && dcn <= dcnCopyRectangle)
		ReleaseMutex(hmMemoryBuffer);
#endif
	setCF(0);
}

/* ====================================================================== */
/*                                                                        */
/*   InitDriver                                                           */
/*                                                                        */
/* ====================================================================== */

void InitDriver()
{
	unsigned long *arguments;
	unsigned short resolutionX, resolutionY;
#if USE_MESSAGE_BOX
	char mess2[40];
#endif

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	resolutionX = (unsigned short)arguments[0];
	resolutionY = (unsigned short)arguments[1];

	InitVideo(resolutionX, resolutionY);

    arguments[0] = ResolutionX;
    arguments[1] = ResolutionY;

    if (ResolutionX>=MIN_RESX&&ResolutionY>=MIN_RESY)
    {
        arguments[0] = ResolutionX/2;
        arguments[1] = ResolutionY/2;
        Zoom = 3;
    }

#if USE_MESSAGE_BOX
	sprintf(mess,"X = %d, Y = %d", arguments[0], arguments[1]);
	sprintf(mess2,"InitDriver(%d,%d)",resolutionX,resolutionY);
	MessageBox(NULL, mess, mess2, MB_OK);
#endif

	Color = 15;
	Scale = 1;
	TopOfWindowStateStack = 0;
	//SavedRectangle = false;
	CharSetSpecial = false;
	CharSetIEEE = false;
	LineDrawingMode = SOLID;
	SetCursorStyle(0);
	SetModeNormal();
}

/* ====================================================================== */
/*                                                                        */
/*   ShutdownDriver                                                       */
/*                                                                        */
/* ====================================================================== */

void ShutdownDriver()
{
#if USE_MESSAGE_BOX
	MessageBox(NULL, NULL, "ShutdownDriver", MB_OK);
#endif
	ShutdownVideo();
}

/* ====================================================================== */
/*                                                                        */
/*   GetScreenResolution												  */
/*                                                                        */
/* ====================================================================== */

void WGetScreenResolution(void)
{
	unsigned long *arguments;
	unsigned short resolutionX, resolutionY;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	resolutionX = (unsigned short)arguments[0];
	resolutionY = (unsigned short)arguments[1];

	hMonitor2Use = NULL;

	rectMonitor.right = 0;
	rectMonitor.left = 0;
	rectMonitor.top = 0;
	rectMonitor.bottom = 0;

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	if (resolutionX)
		resolutionX = ResolutionX;
	else
	if (hMonitor2Use!=NULL)
		resolutionX = rectMonitor.right-rectMonitor.left;
	else
		resolutionX = GetSystemMetrics(SM_CXSCREEN);

	if (ResolutionY)
		resolutionY = ResolutionY;
	else
	if (hMonitor2Use!=NULL)
		resolutionY = rectMonitor.bottom-rectMonitor.top;
	else
		resolutionY = GetSystemMetrics(SM_CYSCREEN);

    if (resolutionX>=MIN_RESX&&resolutionY>=MIN_RESY)
    {
        resolutionX /= 2;
        resolutionY /= 2;
        Zoom = 3;
    }

	arguments[0] = resolutionX - 1;
	arguments[1] = resolutionY - 1;

#if USE_MESSAGE_BOX
	sprintf(mess,"X = %d, Y = %d", arguments[0], arguments[1]);
	MessageBox(NULL, mess, "GetScreenResolution", MB_OK);
#endif
}

/* ====================================================================== */
/*                                                                        */
/*   WGetColor                                                            */
/*                                                                        */
/* ====================================================================== */

void WGetColor()
{
	unsigned char color;

	color = GetColor();
	setAX(color);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetColor                                                            */
/*                                                                        */
/* ====================================================================== */

void WSetColor()
{
	unsigned long *arguments;
	unsigned char index;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	index = (unsigned char)arguments[0];

	SetColor(index);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetCursorStyle                                                      */
/*                                                                        */
/* ====================================================================== */

void WSetCursorStyle()
{
	unsigned long *arguments;
	unsigned char newStyle;
	unsigned char oldStyle;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	newStyle = (unsigned char)arguments[0];

	oldStyle = (unsigned char)SetCursorStyle(newStyle);
	setAX(oldStyle);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetCursorState                                                      */
/*                                                                        */
/* ====================================================================== */

void WSetCursorState()
{
	unsigned long *arguments;
	unsigned char state;
	short x;
	short y;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	state = (unsigned char)arguments[0];
	x = (short)arguments[1];
	y = (short)arguments[2];

	SetCursorState(state, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawWideLineAbsolute                                                */
/*                                                                        */
/* ====================================================================== */

void WDrawWideLineAbsolute()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	DrawWideLineAbsolute(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawWideLineRelative                                                */
/*                                                                        */
/* ====================================================================== */

void WDrawWideLineRelative()
{
	unsigned long *arguments;
	short dx;
	short dy;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	dx = (short)arguments[0];
	dy = (short)arguments[1];

	DrawWideLineRelative(dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawDashedLineAbsolute                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawDashedLineAbsolute()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	DrawDashedLineAbsolute(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawDashedLineRelative                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawDashedLineRelative()
{
	unsigned long *arguments;
	short dx;
	short dy;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	dx = (short)arguments[0];
	dy = (short)arguments[1];

	DrawDashedLineRelative(dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawSolidLineAbsolute                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawSolidLineAbsolute()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	DrawSolidLineAbsolute(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawSolidLineRelative                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawSolidLineRelative()
{
	unsigned long *arguments;
	short dx;
	short dy;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	dx = (short)arguments[0];
	dy = (short)arguments[1];

	DrawSolidLineRelative(dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawGridDots                                                         */
/*                                                                        */
/* ====================================================================== */

void WDrawGridDots()
{
	unsigned long *arguments;
	unsigned short spacing;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	spacing = (unsigned short)arguments[0];

	DrawGridDots(spacing);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawGridDotsRelative                                                 */
/*                                                                        */
/* ====================================================================== */

void WDrawGridDotsRelative()
{
	unsigned long *arguments;
	unsigned short spacing;
	unsigned short dx, dy;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	spacing = (unsigned short)arguments[0];
	dx = (unsigned short)arguments[1];
	dy = (unsigned short)arguments[2];

	DrawGridDotsRelative(spacing, dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetWindow32                                                         */
/*                                                                        */
/* ====================================================================== */

void WSetWindow32()
{
	unsigned long *arguments;
	long minX;
	long minY;
	long maxX;
	long maxY;
	unsigned long scale;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	minX = (long)arguments[0];
	minY = (long)arguments[1];
	maxX = (long)arguments[2];
	maxY = (long)arguments[3];
	scale = arguments[4];

	SetWindow32(minX, minY, maxX, maxY, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetWindow                                                           */
/*                                                                        */
/* ====================================================================== */

void WSetWindow()
{
	unsigned long *arguments;
	short minX;
	short minY;
	short maxX;
	short maxY;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	minX = (short)arguments[0];
	minY = (short)arguments[1];
	maxX = (short)arguments[2];
	maxY = (short)arguments[3];

	SetWindow(minX, minY, maxX, maxY);
}

/* ====================================================================== */
/*                                                                        */
/*   WGetScale                                                            */
/*                                                                        */
/* ====================================================================== */

void WGetScale()
{
	unsigned short scale;

	scale = GetScale();
	setAX(scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetScale                                                            */
/*                                                                        */
/* ====================================================================== */

void WSetScale()
{
	unsigned long *arguments;
	unsigned short scale;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	scale = (unsigned short)arguments[0];

	SetScale(scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WSetWindowOrigin                                                     */
/*                                                                        */
/* ====================================================================== */

void WSetWindowOrigin()
{
	unsigned long *arguments;
	unsigned short originX;
	unsigned short originY;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	originX = (unsigned short)arguments[0];
	originY = (unsigned short)arguments[1];

	SetWindowOrigin(originX, originY);
}

/* ====================================================================== */
/*                                                                        */
/*   WMoveAbsolute                                                        */
/*                                                                        */
/* ====================================================================== */

void WMoveAbsolute()
{
	unsigned long *arguments;
	short x;
	short y;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	x = (short)arguments[0];
	y = (short)arguments[1];

	MoveAbsolute(x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   WMoveRelative                                                        */
/*                                                                        */
/* ====================================================================== */

void WMoveRelative()
{
	unsigned long *arguments;
	short dx;
	short dy;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	dx = (short)arguments[0];
	dy = (short)arguments[1];

	MoveRelative(dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WClearRectangle                                                      */
/*                                                                        */
/* ====================================================================== */

void WClearRectangle()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	ClearRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WFillRectangle                                                       */
/*                                                                        */
/* ====================================================================== */

void WFillRectangle()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	FillRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WHighlightRectangle                                                  */
/*                                                                        */
/* ====================================================================== */

void WHighlightRectangle()
{
	unsigned long *arguments;
	short xa;
	short ya;
	short xb;
	short yb;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (short)arguments[0];
	ya = (short)arguments[1];
	xb = (short)arguments[2];
	yb = (short)arguments[3];

	HighlightRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawArcAbsolute                                                     */
/*                                                                        */
/* ====================================================================== */

void WDrawArcAbsolute()
{
	unsigned long *arguments;
	short xc;
	short yc;
	short xa;
	short ya;
	short xb;
	short yb;
	unsigned short radius;
	unsigned short thickness;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xc = (short)arguments[0];
	yc = (short)arguments[1];
	xa = (short)arguments[2];
	ya = (short)arguments[3];
	xb = (short)arguments[4];
	yb = (short)arguments[5];
	radius = (unsigned short)arguments[6];
	thickness = (unsigned short)arguments[7];

	DrawArcAbsolute(xc, yc, xa, ya, xb, yb, radius, thickness);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawWideArcAbsolute32                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawWideArcAbsolute32()
{
	unsigned long *arguments;
	long xc;
	long yc;
	long xa;
	long ya;
	long xb;
	long yb;
	unsigned long radius;
	unsigned long thickness;
	unsigned char fillMode;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xc = (long)arguments[0];
	yc = (long)arguments[1];
	xa = (long)arguments[2];
	ya = (long)arguments[3];
	xb = (long)arguments[4];
	yb = (long)arguments[5];
	radius = (unsigned long)arguments[6];
	thickness = (unsigned long)arguments[7];
	fillMode = (unsigned char)arguments[8];

	DrawWideArcAbsolute32(xc, yc, xa, ya, xb, yb, radius, thickness,
		fillMode);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawCircleAbsolute                                                  */
/*                                                                        */
/* ====================================================================== */

void WDrawCircleAbsolute()
{
	unsigned long *arguments;
	short xc;
	short yc;
	unsigned short length;
	unsigned short height;
	unsigned char fill;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xc = (short)arguments[0];
	yc = (short)arguments[1];
	length = (unsigned short)arguments[2];
	height = (unsigned short)arguments[3];
	fill = (unsigned char)arguments[4];

	DrawCircleAbsolute(xc, yc, length, height, fill);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawCircleRelative                                                  */
/*                                                                        */
/* ====================================================================== */

void WDrawCircleRelative()
{
	unsigned long *arguments;
	short dx;
	short dy;
	unsigned short length;
	unsigned short height;
	unsigned char fill;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	dx = (short)arguments[0];
	dy = (short)arguments[1];
	length = (unsigned short)arguments[2];
	height = (unsigned short)arguments[3];
	fill = (unsigned char)arguments[4];

	DrawCircleRelative(dx, dy, length, height, fill);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawCircleAbsolute32                                                */
/*                                                                        */
/* ====================================================================== */

void WDrawCircleAbsolute32()
{
	unsigned long *arguments;
	long xc;
	long yc;
	unsigned long radius;
	unsigned char fill;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xc = (long)arguments[0];
	yc = (long)arguments[1];
	radius = (unsigned long)arguments[2];
	fill = (unsigned char)arguments[3];

	DrawCircleAbsolute32(xc, yc, radius, fill);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawBitmapRelative                                                  */
/*                                                                        */
/* ====================================================================== */

void WDrawBitmapRelative()
{
	unsigned long *arguments;
	unsigned short BitmapSegment;
	unsigned short BitmapOffset;
	unsigned short rows;
	unsigned short cols;
	unsigned short dx;
	unsigned short dy;
	unsigned short xsize;
	unsigned short ysize;
	unsigned char function;
	unsigned char *bitmap;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	BitmapSegment = (unsigned short)arguments[0];
	BitmapOffset = (unsigned short)arguments[1];
	rows = (unsigned short)arguments[2];
	cols = (unsigned short)arguments[3];
	dx = (unsigned short)arguments[4];
	dy = (unsigned short)arguments[5];
	xsize = (unsigned short)arguments[6];
	ysize = (unsigned short)arguments[7];
	function = (unsigned short)arguments[8];

	bitmap = (unsigned char *)VdmMapFlat(BitmapSegment, BitmapOffset,
		VDM_V86);

	DrawBitmapRelative(bitmap, rows, cols, dx, dy, xsize, ysize,
		function);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawWideLineAbsolute32                                              */
/*                                                                        */
/* ====================================================================== */

void WDrawWideLineAbsolute32()
{
	unsigned long *arguments;
	long xa;
	long ya;
	long xb;
	long yb;
	unsigned long thickness;
	unsigned char fill;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);
	xa = (long)arguments[0];
	ya = (long)arguments[1];
	xb = (long)arguments[2];
	yb = (long)arguments[3];
	thickness = (unsigned long)arguments[4];
	fill = (unsigned char)arguments[5];

	DrawWideLineAbsolute32(xa, ya, xb, yb, thickness, fill);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawFixedStringAbsolute                                             */
/*                                                                        */
/* ====================================================================== */

void WDrawFixedStringAbsolute()
{
	unsigned long *arguments;
	short length;
	unsigned short stringSegment;
	unsigned short stringOffset;
	short x;
	short y;
	unsigned char *string;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	length = (short)arguments[0];
	stringSegment = (unsigned short)arguments[1];
	stringOffset = (unsigned short)arguments[2];
	x = (short)arguments[3];
	y = (short)arguments[4];
	string = (unsigned char *)VdmMapFlat(stringSegment, stringOffset,
		VDM_V86);

	DrawFixedStringAbsolute(length, string, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawFixedStringRelative                                             */
/*                                                                        */
/* ====================================================================== */

void WDrawFixedStringRelative()
{
	unsigned long *arguments;
	short length;
	unsigned short stringSegment;
	unsigned short stringOffset;
	short dx;
	short dy;
	unsigned char *string;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	length = (short)arguments[0];
	stringSegment = (unsigned short)arguments[1];
	stringOffset = (unsigned short)arguments[2];
	dx = (short)arguments[3];
	dy = (short)arguments[4];
	string = (unsigned char *)VdmMapFlat(stringSegment, stringOffset,
		VDM_V86);

	DrawFixedStringRelative(length, string, dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawVariableStringAbsolute                                          */
/*                                                                        */
/* ====================================================================== */

void WDrawVariableStringAbsolute()
{
	unsigned long *arguments;
	short length;
	unsigned short stringSegment;
	unsigned short stringOffset;
	short x;
	short y;
	short scale;
	unsigned char *string;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	length = (short)arguments[0];
	stringSegment = (unsigned short)arguments[1];
	stringOffset = (unsigned short)arguments[2];
	x = (short)arguments[3];
	y = (short)arguments[4];
	scale = (short)arguments[5];
	string = (unsigned char *)VdmMapFlat(stringSegment, stringOffset,
		VDM_V86);

	DrawVariableStringAbsolute(length, string, x, y, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawVariableStringRelative                                          */
/*                                                                        */
/* ====================================================================== */

void WDrawVariableStringRelative()
{
	unsigned long *arguments;
	short length;
	unsigned short stringSegment;
	unsigned short stringOffset;
	short dx;
	short dy;
	short scale;
	unsigned char *string;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	length = (short)arguments[0];
	stringSegment = (unsigned short)arguments[1];
	stringOffset = (unsigned short)arguments[2];
	dx = (short)arguments[3];
	dy = (short)arguments[4];
	scale = (short)arguments[5];
	string = (unsigned char *)VdmMapFlat(stringSegment, stringOffset,
		VDM_V86);

	DrawVariableStringRelative(length, string, dx, dy, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawFixedTextAbsolute                                             */
/*                                                                        */
/* ====================================================================== */

void WDrawFixedTextAbsolute()
{
	unsigned long *arguments;
	unsigned short textSegment;
	unsigned short textOffset;
	short x;
	short y;
	unsigned char *text;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	textSegment = (unsigned short)arguments[0];
	textOffset = (unsigned short)arguments[1];
	x = (short)arguments[2];
	y = (short)arguments[3];
	text = (unsigned char *)VdmMapFlat(textSegment, textOffset, VDM_V86);

	DrawFixedTextAbsolute(text, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawFixedTextRelative                                               */
/*                                                                        */
/* ====================================================================== */

void WDrawFixedTextRelative()
{
	unsigned long *arguments;
	unsigned short textSegment;
	unsigned short textOffset;
	short dx;
	short dy;
	unsigned char *text;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	textSegment = (unsigned short)arguments[0];
	textOffset = (unsigned short)arguments[1];
	dx = (short)arguments[2];
	dy = (short)arguments[3];
	text = (unsigned char *)VdmMapFlat(textSegment, textOffset, VDM_V86);

	DrawFixedTextRelative(text, dx, dy);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawVariableTextAbsolute                                            */
/*                                                                        */
/* ====================================================================== */

void WDrawVariableTextAbsolute()
{
	unsigned long *arguments;
	unsigned short textSegment;
	unsigned short textOffset;
	short x;
	short y;
	short scale;
	unsigned char *text;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	textSegment = (unsigned short)arguments[0];
	textOffset = (unsigned short)arguments[1];
	x = (short)arguments[2];
	y = (short)arguments[3];
	scale = (short)arguments[4];
	text = (unsigned char *)VdmMapFlat(textSegment, textOffset, VDM_V86);

	DrawVariableTextAbsolute(text, x, y, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WDrawVariableTextRelative                                            */
/*                                                                        */
/* ====================================================================== */

void WDrawVariableTextRelative()
{
	unsigned long *arguments;
	unsigned short textSegment;
	unsigned short textOffset;
	short dx;
	short dy;
	short scale;
	unsigned char *text;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	textSegment = (unsigned short)arguments[0];
	textOffset = (unsigned short)arguments[1];
	dx = (short)arguments[2];
	dy = (short)arguments[3];
	scale = (short)arguments[4];
	text = (unsigned char *)VdmMapFlat(textSegment, textOffset, VDM_V86);

	DrawVariableTextRelative(text, dx, dy, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   WSaveRectangle											              */
/*                                                                        */
/* ====================================================================== */

void WSaveRectangle(void)
{
	short xa, ya, xb, yb;
	unsigned long *arguments;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	xa = arguments[0];
	ya = arguments[1];
	xb = arguments[2];
	yb = arguments[3];

	SaveRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WCopyRectangle											              */
/*                                                                        */
/* ====================================================================== */

void WCopyRectangle(void)
{
	short xa, ya, xb, yb;
	unsigned long *arguments;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	xa = arguments[0];
	ya = arguments[1];
	xb = arguments[2];
	yb = arguments[3];

	CopyRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   WKeybrdServe                                                         */
/*                                                                        */
/* ====================================================================== */

void WKeybrdServe(void)
{
	unsigned long *arguments;
	union REGPACK regs;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	regs.w.ax = arguments[0];
	regs.w.bx = arguments[1];
	regs.w.flags = arguments[2];

	KeybrdServe(&regs);

	arguments[0] = regs.w.ax;
	arguments[1] = regs.w.bx;
	arguments[2] = regs.w.flags;
}

/* ====================================================================== */
/*                                                                        */
/*   WMouseServe                                                          */
/*                                                                        */
/* ====================================================================== */

void WMouseServe(void)
{
	unsigned long *arguments;
	union REGPACK regs;

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	regs.w.ax = arguments[0];
	regs.w.bx = arguments[1];
	regs.w.cx = arguments[2];
	regs.w.dx = arguments[3];

	MouseServe(&regs);

	arguments[0] = regs.w.ax;
	arguments[1] = regs.w.bx;
	arguments[2] = regs.w.cx;
	arguments[3] = regs.w.dx;
}

/* ====================================================================== */
/*                                                                        */
/*   WVideoServe                                                          */
/*                                                                        */
/* ====================================================================== */

void WVideoServe(void)
{
	unsigned long *arguments;
	union REGPACK regs;
#if USE_MESSAGE_BOX
	char mess2[40];
#endif

	arguments = (unsigned long *)VdmMapFlat(getES(), getDI(), VDM_V86);

	regs.w.ax = arguments[0];
	regs.w.bx = arguments[1];
	regs.w.cx = arguments[2];
	regs.w.dx = arguments[3];
	regs.w.bp = arguments[4];
	regs.w.es = arguments[5];

#if USE_MESSAGE_BOX
	sprintf(mess2,"AX = 0x%X, BX = 0x%X", regs.w.ax, regs.w.bx);
	MessageBox(NULL, mess2, "Video BIOS Int10h", MB_OK);
#else
	VideoInt10Serve(&regs);
#endif

	arguments[0] = regs.w.ax;
	arguments[1] = regs.w.bx;
	arguments[2] = regs.w.cx;
	arguments[3] = regs.w.dx;
	arguments[4] = regs.w.bp;
	arguments[5] = regs.w.es;
}

/* ====================================================================== */
/*                                                                        */
/*   WDisplayMessage                                                      */
/*                                                                        */
/* ====================================================================== */

void WDisplayMessage(void)
{
	char *Message;
	Message = (char *)VdmMapFlat(getES(), getDI(), VDM_V86);
	MessageBox(NULL, NULL, Message, MB_OK);
}

/* -- END --------------------------------------------------------------- */
