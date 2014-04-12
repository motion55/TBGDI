/* ********************************************************************** */
/*                                                                        */
/*   CURSOR.C                                                             */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "cursor.h"

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

unsigned char CursorStyle = 0;
unsigned char CursorState = 0;
unsigned short CursorScreenX = 0;
unsigned short CursorScreenY = 0;

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static HDC hdcSavedCursor;
static HBITMAP hbmSavedCursor;

static unsigned char Cursor[CURSOR_VSIZE][CURSOR_HSIZE] = {
	0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff,
	0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00,
	0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x0f, 0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff,
	0x0f, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff,
	0x00, 0xff, 0xff, 0x00, 0x0f, 0x0f, 0x00, 0xff,
	0xff, 0xff, 0xff, 0x00, 0x0f, 0x0f, 0x00, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0x0f, 0x00,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0x0f, 0x00,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
};

static COLORREF CursorBackground[CURSOR_VSIZE][CURSOR_HSIZE];

static unsigned short CursorPositionX;
static unsigned short CursorPositionY;
static unsigned int CursorVisibleColumns = 0;
static unsigned int CursorVisibleRows = 0;

/* -- PRIVATE FUNCTION DECLARATIONS ------------------------------------- */

static void DrawCursor(unsigned short x, unsigned short y,
	unsigned int cols, unsigned int rows);
static void SaveCursorBackground(unsigned short x, unsigned short y,
	unsigned int cols, unsigned int rows);
static void RestoreCursorBackground(short x, short y,
	unsigned int cols, unsigned int rows);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   LoadCursor													          */
/*                                                                        */
/* ====================================================================== */

void InitCursor(void)
{
	hdcSavedCursor = CreateCompatibleDC(hdcWindow);
	hbmSavedCursor = CreateCompatibleBitmap(hdcWindow,
		CURSOR_HSIZE, CURSOR_VSIZE);
	SelectObject(hdcSavedCursor, hbmSavedCursor);
}

/* ====================================================================== */
/*                                                                        */
/*   UnloadCursor													      */
/*                                                                        */
/* ====================================================================== */

void UnloadCursor(void)
{
	DeleteDC(hdcSavedCursor);
	DeleteObject(hbmSavedCursor);
}

/* ====================================================================== */
/*                                                                        */
/*   SetCursorState                                                       */
/*                                                                        */
/* ====================================================================== */

void SetCursorState(unsigned char state, short x, short y)
{
	if ((CursorVisibleRows > 0) && (CursorVisibleRows > 0)) {
		RestoreCursorBackground(CursorPositionX, CursorPositionY,
			CursorVisibleColumns, CursorVisibleRows);
	}

	if (CursorStyle != 0) {
		SetModeXor();
		SetColor(8);

		if (CursorState != 0) {
			if ((state == 0) || (x != CursorPositionX)) {
				DrawLineFunction(CursorPositionX,
					WindowStartY,
					CursorPositionX,
					WindowEndY);
			}

			if ((state == 0) || (y != CursorPositionY)) {
				DrawLineFunction(WindowStartX,
					CursorPositionY,
					WindowEndX,
					CursorPositionY);
			}
		}

		if (state != 0) {
			if (x != CursorPositionX) {
				DrawLineFunction(x, WindowStartY,
					x, WindowEndY);
			}

			if (y != CursorPositionY) {
				DrawLineFunction(WindowStartX, y,
					WindowEndX, y);
			}
		}

		SetModeNormal();
	}

	if (state != 0) {
		CursorPositionX = x;
		CursorPositionY = y;

		if ((x < WindowStartX) || (y < WindowStartY)) {
			CursorVisibleColumns = 0;
			CursorVisibleRows = 0;
			return;
		}

		Translate(&x, &y);

		if ((x >= ResolutionX) || (y >= ResolutionY)) {
			CursorVisibleColumns = 0;
			CursorVisibleRows = 0;
			return;
		}

		if (x + 7 >= ResolutionX) {
			CursorVisibleColumns = (ResolutionX - 1) - x;
		} else {
			CursorVisibleColumns = 8;
		}

		if (y + 14 >= ResolutionY) {
			CursorVisibleRows = (ResolutionY - 1) - y;
		} else {
			CursorVisibleRows = 15;
		}

		SaveCursorBackground(x, y,
			CursorVisibleColumns, CursorVisibleRows);
		DrawCursor(x, y,
			CursorVisibleColumns, CursorVisibleRows);
	} else {
		CursorPositionX = -1;
		CursorPositionY = -1;
		CursorVisibleColumns = 0;
		CursorVisibleRows = 0;
	}

	CursorState = state;
}

/* ====================================================================== */
/*                                                                        */
/*   SetCursorStyle                                                       */
/*                                                                        */
/* ====================================================================== */

unsigned char SetCursorStyle(unsigned char newStyle)
{
	unsigned char oldStyle;

	SetCursorState(0, 0, 0);
	oldStyle = CursorStyle;
	CursorStyle = newStyle;
	return oldStyle;
}

/* ====================================================================== */
/*                                                                        */
/*   ZoomScroll                                                           */
/*                                                                        */
/* ====================================================================== */
void ZoomScroll(void)
{
	short X, Y;

	X = (CursorScreenX-ZoomScrollX) * ZOOM;
	while (X>=ResolutionX-(2*CURSOR_HSIZE))
	{
		if (ZoomScrollX < (ResolutionX/2))
		{
			ZoomScrollX += ResolutionX/4;
			X = (CursorScreenX-ZoomScrollX) * ZOOM;
			RedrawTimer = 1;
		}
		else break;
	}

	while (X<0)
	{
		if (ZoomScrollX >= (ResolutionX/4))
		{
			ZoomScrollX -= ResolutionX/4;
			X = (CursorScreenX-ZoomScrollX) * ZOOM;
			RedrawTimer = 1;
		}
		else break;
	}

	Y = (CursorScreenY-ZoomScrollY) * ZOOM;
	while (Y>=ResolutionY-(2*CURSOR_VSIZE))
	{
		if (ZoomScrollY < (ResolutionY/2))
		{
			ZoomScrollY += ResolutionY/4;
			Y = (CursorScreenY-ZoomScrollY) * ZOOM;
			RedrawTimer = 1;
		}
		else break;
	}

	while (Y<0)
	{
		if (ZoomScrollY >= (ResolutionY/4))
		{
			ZoomScrollY -= ResolutionY/4;
			Y = (CursorScreenY-ZoomScrollY) * ZOOM;
			RedrawTimer = 1;
		}
		else break;
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawCursor                                                           */
/*                                                                        */
/* ====================================================================== */

void DrawCursor(unsigned short x, unsigned short y,
	unsigned int cols, unsigned int rows)
{
	unsigned int i;
	unsigned int j;
	unsigned char *cursorData;
	unsigned char pixel;
	int oldRop;

	short X, Y;

	BeginDraw();

	oldRop = SetROP2(hdcMemoryBuffer, R2_COPYPEN);
	for (j = 0; j < rows; j++) {
		cursorData = Cursor[j];
		for (i = 0; i < cols; i++) {
			pixel = *cursorData++;
			if (pixel != 0xff) {
				SetPixel(hdcMemoryBuffer, (x + i), (y + j),
					RGBPalette[pixel]);
			}
		}
	}
	SetROP2(hdcMemoryBuffer, oldRop);

	if (Zoom>1)
	{
		ZoomScroll();

		X = (x - ZoomScrollX) * ZOOM;
		Y = (y - ZoomScrollY) * ZOOM;

		StretchBlt(hdcScaledBuffer, X, Y, cols*ZOOM, rows*ZOOM,
			hdcMemoryBuffer, x, y, cols, rows, SRCCOPY);
		BitBlt(hdcWindow, X, Y, cols*ZOOM, rows*ZOOM,
			hdcScaledBuffer, X, Y, SRCCOPY);
	}
	else
		BitBlt(hdcWindow, x, y, cols, rows, hdcMemoryBuffer, x, y, SRCCOPY);

	if (bDelayDraw)
		RedrawTimer=2;

	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   SaveCursorBackground                                                 */
/*                                                                        */
/* ====================================================================== */

void SaveCursorBackground(unsigned short x, unsigned short y,
	unsigned int cols, unsigned int rows)
{
	CursorScreenX = x;
	CursorScreenY = y;

	BeginDraw();
	BitBlt(hdcSavedCursor, 0, 0, cols, rows,
		hdcMemoryBuffer, x, y, SRCCOPY);
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   RestoreCursorBackground                                              */
/*                                                                        */
/* ====================================================================== */

void RestoreCursorBackground(short x, short y,
	unsigned int cols, unsigned int rows)
{
	short X, Y;

	Translate(&x, &y);

	BeginDraw();
	BitBlt(hdcMemoryBuffer, x, y, cols, rows,
		hdcSavedCursor, 0, 0, SRCCOPY);
	if (Zoom>1)
	{
		ZoomScroll();

		X = (x - ZoomScrollX) * ZOOM;
		Y = (y - ZoomScrollY) * ZOOM;

		StretchBlt(hdcScaledBuffer, X, Y, cols*ZOOM, rows*ZOOM,
			hdcMemoryBuffer, x, y, cols, rows, SRCCOPY);
		BitBlt(hdcWindow, X, Y, cols*ZOOM, rows*ZOOM,
			hdcScaledBuffer, X, Y, SRCCOPY);
	}
	else
		BitBlt(hdcWindow, x, y, cols, rows,
			hdcSavedCursor, 0, 0, SRCCOPY);
	EndDraw();
}

/* -- END --------------------------------------------------------------- */
