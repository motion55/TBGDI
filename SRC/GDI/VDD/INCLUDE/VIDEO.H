/* ********************************************************************** */
/*                                                                        */
/*   VIDEO.H                                                              */
/*                                                                        */
/* ********************************************************************** */

#ifndef __VIDEO_H__
#define __VIDEO_H__

/* -- HEADER FILES ------------------------------------------------------ */

#include <windows.h>
#include <windowsx.h>
#include "global.h"
#include "driver.h"
#include "state.h"

/* -- PUBLIC DATA DECLARATIONS ------------------------------------------ */

extern HWND hWnd;
extern HDC hdcWindow;
extern HANDLE hmMemoryBuffer;
extern HDC hdcMemoryBuffer;
extern HDC hdcScaledBuffer;
extern HANDLE hKeyMouseEvent;

extern BOOL bDelayDraw;
extern int RedrawTimer;
#define Redraw()	{if (!RedrawTimer) RedrawTimer=10;}

#define BeginDraw()	WaitForSingleObject(hmMemoryBuffer, INFINITE)
#define EndDraw()	ReleaseMutex(hmMemoryBuffer)

#define	PUTPIXEL(a, b)	SetPixel(hdcMemoryBuffer, a, b, RGBPalette[MapMask]);

/* -- PUBLIC FUNCTION DECLARATIONS -------------------------------------- */

extern bool InitVideo(unsigned short resX, unsigned short resY);
extern void ShutdownVideo();
extern void Translate(short *const x, short *const y);
extern void PutPixel(short x, short y);

extern void KeybrdServe(union REGPACK *pRegs);
extern void MouseServe(union REGPACK *pRegs);
extern void VideoInt10Serve(union REGPACK *pRegs);

/* -- Used for multi-montor selection ----------------------------------- */

extern HMONITOR hMonitor2Use;
extern RECT rectMonitor;

extern BOOL CALLBACK MonitorEnumProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
);

#endif

/* -- END --------------------------------------------------------------- */
