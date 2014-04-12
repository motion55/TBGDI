/* ********************************************************************** */
/*                                                                        */
/*   RECTNGLE.C                                                           */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "rectngle.h"

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

HDC hdcSavedRect = NULL;
HBITMAP hbmSavedRect;

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static short RectangleStartX;
static short RectangleStartY;
static short RectangleEndX;
static short RectangleEndY;

/* -- PRIVATE FUNCTION DECLARATIONS ------------------------------------- */

static int ClipRectangle(short *xa, short *ya, short *xb, short *yb);

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   ClearRectangle                                                       */
/*                                                                        */
/* ====================================================================== */

void ClearRectangle(short xa, short ya, short xb, short yb)
{
	RECT rect;

	if (ClipRectangle(&xa, &ya, &xb, &yb) == false) {
		return;
	}

	rect.left = xa;
	rect.top = ya;
	rect.right = (xb + 1);
	rect.bottom = (yb + 1);

	BeginDraw();
	FillRect(hdcMemoryBuffer, &rect, GetStockObject(BLACK_BRUSH));
	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
		FillRect(hdcWindow, &rect, GetStockObject(BLACK_BRUSH));
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   FillRectangle                                                        */
/*                                                                        */
/* ====================================================================== */

void FillRectangle(short xa, short ya, short xb, short yb)
{
	RECT rect;
	HBRUSH hBrush;
	HDC hdcTempRect;
	HBITMAP hbmTempRect;

	if (ClipRectangle(&xa, &ya, &xb, &yb) == false) {
		return;
	}

	rect.left = 0;
	rect.top = 0;
	rect.right = xb - xa + 1;
	rect.bottom = yb - ya + 1;

	hdcTempRect = CreateCompatibleDC(hdcWindow);
	hbmTempRect = CreateCompatibleBitmap(hdcWindow, xb-xa+1, yb-ya+1);
	SelectObject(hdcTempRect, hbmTempRect);
	hBrush = CreateSolidBrush(RGBPalette[MapMask]);

	FillRect(hdcTempRect, &rect, hBrush);

	BeginDraw();

	if (Mode == OR)
		BitBlt(hdcMemoryBuffer, xa, ya, xb-xa+1, yb-ya+1, hdcTempRect, 0, 0, SRCPAINT);
	else
		BitBlt(hdcMemoryBuffer, xa, ya, xb-xa+1, yb-ya+1, hdcTempRect, 0, 0, SRCINVERT);

	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
		BitBlt(hdcWindow, xa, ya, xb-xa+1, yb-ya+1, hdcMemoryBuffer, xa, ya, SRCCOPY);

	EndDraw();

	DeleteDC(hdcTempRect);
	DeleteObject(hbmTempRect);
	DeleteObject(hBrush);
}

/* ====================================================================== */
/*                                                                        */
/*   HighlightRectangle                                                   */
/*                                                                        */
/* ====================================================================== */

void HighlightRectangle(short xa, short ya, short xb, short yb)
{
	RECT rect;

	if (ClipRectangle(&xa, &ya, &xb, &yb) == false) {
		return;
	}

	rect.left = xa;
	rect.top = ya;
	rect.right = (xb + 1);
	rect.bottom = (yb + 1);

	BeginDraw();
	InvertRect(hdcMemoryBuffer, &rect);
	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
		InvertRect(hdcWindow, &rect);
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   SaveRectangle                                                        */
/*                                                                        */
/* ====================================================================== */

void SaveRectangle(short xa, short ya, short xb, short yb)
{
	if (hdcSavedRect)
	{
		DeleteDC(hdcSavedRect);
		DeleteObject(hbmSavedRect);
	}

	hdcSavedRect = CreateCompatibleDC(hdcWindow);
	hbmSavedRect = CreateCompatibleBitmap(hdcWindow, xb-xa+1, yb-ya+1);
	SelectObject(hdcSavedRect, hbmSavedRect);

	BeginDraw();
	BitBlt(hdcSavedRect, 0, 0, xb-xa+1, yb-ya+1,
		hdcMemoryBuffer, xa, ya, SRCCOPY);
	EndDraw();

	RectangleStartX = xa;
	RectangleStartY = ya;
	RectangleEndX = xb;
	RectangleEndY = yb;
}

/* ====================================================================== */
/*                                                                        */
/*   RestoreRectangle                                                     */
/*                                                                        */
/* ====================================================================== */

void RestoreRectangle()
{
	if (hdcSavedRect == NULL)
		return;

	BeginDraw();

	BitBlt(hdcMemoryBuffer, RectangleStartX, RectangleStartY,
		GetDeviceCaps(hdcSavedRect, HORZRES),
		GetDeviceCaps(hdcSavedRect, VERTRES),
		hdcSavedRect, 0, 0, SRCCOPY);

	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
	{
		BitBlt(hdcWindow, RectangleStartX, RectangleStartY,
			GetDeviceCaps(hdcSavedRect, HORZRES),
			GetDeviceCaps(hdcSavedRect, VERTRES),
			hdcSavedRect, 0, 0, SRCCOPY);
	}

	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   CopyRectangle                                                        */
/*                                                                        */
/* ====================================================================== */

void CopyRectangle(short xa, short ya, short xb, short yb)
{
	SaveRectangle(xa, ya, xb, yb);
}

/* ====================================================================== */
/*                                                                        */
/*   ClipRectangle                                                        */
/*                                                                        */
/* ====================================================================== */

int ClipRectangle(short *xa, short *ya,	short *xb, short *yb)
{
	if ((*xa > *xb) || (*ya > *yb)) {
		return false;
	}

	if ((*xa > WindowEndX) || (*xb < WindowStartX) ||
	    (*ya > WindowEndY) || (*yb < WindowStartY )) {
		return false;
	}

	if (*xa < WindowStartX) {
		*xa = WindowStartX;
	}

	if (*xb > WindowEndX) {
		*xb = WindowEndX;
	}

	if (*ya < WindowStartY ) {
		*ya = WindowStartY ;
	}

	if (*yb > WindowEndY) {
		*yb = WindowEndY;
	}

	Translate(xa, ya);
	Translate(xb, yb);
	return true;
}

/* -- END --------------------------------------------------------------- */
