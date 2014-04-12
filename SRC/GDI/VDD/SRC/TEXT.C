/* ********************************************************************** */
/*                                                                        */
/*   TEXT.C                                                               */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "text.h"

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static unsigned char TextBuffer[256];

/* -- PRIVATE FUNCTION DECLARATIONS ------------------------------------- */

static void DrawFixedString(const unsigned char *string,
	unsigned short length, short xa, short ya);
static void DrawFixedCharacter(unsigned char ch, short x, short y,
	int firstRow, int lastRow, int firstColumn, int lastColumn);
static void DrawVariableCharacter(unsigned char ch,
	short *x, short *y, short scale);

/* -- CODE -------------------------------------------------------------- */

#define	PUTPIXEL(a, b)	SetPixel(hdcMemoryBuffer, a, b, RGBPalette[MapMask]);

/* ====================================================================== */
/*                                                                        */
/*   PrintDriverInfo                                                      */
/*                                                                        */
/* ====================================================================== */

/* verified */
void PrintDriverInfo()
{
	char *p;
	unsigned short size;

	size = 0;
	for (p = DriverInfoString; *p != 0; p++) {
		size++;
	}

	/*
	 * as long as DriverInfoString does not contain non-ASCII
	 * characters, this implementation is identical to the
	 * reference implementation which skipped them
	 *
	 * Attention : Window[Start,End][X,Y] and Scale are still
	 * evaluated, although this is wrong!
	 */
	DrawFixedString((unsigned char *)DriverInfoString, size, 0, 28);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedStringAbsolute                                              */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawFixedStringAbsolute(unsigned short length,
	const unsigned char *string, short x, short y)
{
	unsigned short i;

	DrawingPositionX = x;
	DrawingPositionY = y;

	if (Scale != 1) {
		return;
	}

	if (length < 255) {
		length++;
	} else {
		length = 255;
	}

	for (i = 0; i < length; i++) {
		if (string[i] == 0) {
			break;
		}
	}

	DrawFixedString(string, i, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedStringRelative                                              */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawFixedStringRelative(unsigned short length,
	const unsigned char *string, short dx, short dy)
{
	short x;
	short y;

	x = DrawingPositionX + dx;
	y = DrawingPositionY + dy;

	DrawFixedStringAbsolute(length, string, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableStringAbsolute                                           */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawVariableStringAbsolute(unsigned short length,
	const unsigned char *string, short x, short y, short scale)
{
	unsigned short i;
	unsigned char ch;

	DrawingPositionX = x;
	DrawingPositionY = y;

	if (length < 255) {
		length++;
	} else {
		length = 255;
	}

	for (i = 0; i < length; i++) {
		ch = string[i];

		if (ch == 0) {
			break;
		}

		TextBuffer[i + 1] = ch;
	}

	TextBuffer[0] = i;

	/*
	 * Dangerous! DrawVariableTextAbsolute parses the TextBuffer
	 * past its end for certain control codes. Do not call
	 * DrawVariableTextAbsolute if length == 0
	 */
	DrawVariableTextAbsolute(TextBuffer, x, y, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableStringRelative                                           */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawVariableStringRelative(unsigned short length,
	const unsigned char *string, short dx, short dy, short scale)
{
	short x;
	short y;

	x = DrawingPositionX + dx;
	y = DrawingPositionY + dy;

	DrawVariableStringAbsolute(length, string, x, y, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedTextAbsolute                                                */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawFixedTextAbsolute(const unsigned char *text, short x, short y)
{
	DrawingPositionX = x;
	DrawingPositionY = y;

	DrawFixedString((text + 1), *text, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedTextRelative                                                */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawFixedTextRelative(const unsigned char *text, short dx, short dy)
{
	short x;
	short y;

	x = DrawingPositionX + dx;
	y = DrawingPositionY + dy;

	DrawFixedTextAbsolute(text, x, y);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableTextAbsolute                                             */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawVariableTextAbsolute(const unsigned char *text,
	short x, short y, short scale)
{
	if (text[0] == 0) {
		if (text[1] == 0xff) {
			CharSetIEEE = true;
		} else if (text[1] == 0xfe) {
			CharSetSpecial = true;
		} else {
			CharSetIEEE = false;
			CharSetSpecial = false;
		}

		return;
	}

	DrawingPositionX = x;
	DrawingPositionY = y;

	DrawVariableTextRelative(text, 0, 0, scale);
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableTextRelative                                             */
/*                                                                        */
/* ====================================================================== */

/* verified */
void DrawVariableTextRelative(const unsigned char *text,
	short dx, short dy, short scale)
{
	short offsetX;
	short offsetY;
	short x;
	short y;
	unsigned char i;

	DrawingPositionX += dx;
	DrawingPositionY += dy;

	if ((scale != 0) && (text[0] > 0)) {
		if ((scale == 1) && (Scale == 1)
		&&  ((text[0] != 1) || (text[1] > 0x20)))
		{
			DrawFixedTextRelative(text, 0, 0);
			CharSetIEEE = false;
			CharSetSpecial = false;
			return;
		}

		if (scale < 0) {
			scale = -scale;
			offsetX = -(scale * 8);
			offsetY = scale * 10;
		} else {
			offsetX = 0;
			offsetY = 0;
		}

		x = DrawingPositionX;
		y = DrawingPositionY;

		for (i = 0; i < text[0]; i++) {
			DrawVariableCharacter(text[i + 1], &x, &y, scale);
			x += offsetX;
			y += offsetY;
		}
	}

	CharSetIEEE = false;
	CharSetSpecial = false;
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedString                                                      */
/*                                                                        */
/* ====================================================================== */

void DrawFixedString(const unsigned char *string, unsigned short length,
	short xa, short ya)
{
	short xb;
	short yb;
	short x;
	short y;
	int firstRow;
	int lastRow;
	int firstColumn;
	int lastColumn;
	int firstCharacter;
	int lastCharacter;
	int i;

	xb = xa + (length * 8) - 1;
	yb = ya - 9;

	x = xa;
	y = yb;

	if ((xa > WindowEndX) || (xb < WindowStartX)
	||  (yb > WindowEndY) || (ya < WindowStartY)
	||  (length == 0) || (Scale != 1))
	{
		return;
	}

	if (xa < WindowStartX) {
		firstCharacter = ((WindowStartX - xa) >> 3);
		firstColumn = ((WindowStartX - xa) & 0x7);
		x = xa + ((WindowStartX - xa) & ~0x7);
	} else {
		firstCharacter = 0;
		firstColumn = 0;
	}

	if (xb > WindowEndX) {
		lastCharacter = length - ((xb - WindowEndX) >> 3) - 1;
		lastColumn = 7 - ((xb - WindowEndX) & 0x7);
	} else {
		lastCharacter = length - 1;
		lastColumn = 7;
	}

	if (yb < WindowStartY) {
		firstRow = WindowStartY - yb;
	} else {
		firstRow = 0;
	}

	if (ya > WindowEndY) {
		lastRow = 9 - (ya - WindowEndY);
	} else {
		lastRow = 9;
	}

	Translate(&x, &y);
	string += firstCharacter;

	if (lastCharacter == firstCharacter) {
		DrawFixedCharacter(*string, x, y, firstRow, lastRow,
			firstColumn, lastColumn);
	} else {
		DrawFixedCharacter(*string, x, y, firstRow, lastRow,
			firstColumn, 7);
		string++;
		x += 8;

		for (i = (firstCharacter + 1); i < lastCharacter; i++) {
			DrawFixedCharacter(*string, x, y, firstRow,
				lastRow, 0, 7);
			string++;
			x += 8;
		}

		DrawFixedCharacter(*string, x, y, firstRow, lastRow,
			0, lastColumn);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   DrawFixedCharacter                                                   */
/*                                                                        */
/* ====================================================================== */

void DrawFixedCharacter(unsigned char ch, short x, short y,
	int firstRow, int lastRow, int firstColumn, int lastColumn)
{
	int i;
	int j;
	unsigned char fontData;
	unsigned char pixel;

	if (ch < 0x20) {
		return;
	}

	BeginDraw();
	if ((CharSetSpecial == false) && (ch & 0x80)) {
		ch &= 0x7f;

		if (firstRow == 0) {
			for (i = firstColumn; i <= lastColumn; i++) {
				PUTPIXEL((x + i), y);
			}
		}
	}

	ch -= 0x20;

	if (firstRow < 2) {
		firstRow = 2;
	}

	for (j = firstRow; j <= lastRow; j++) {
		fontData = BitmapFont[ch][j - 2];
		for (i = firstColumn; i <= lastColumn; i++) {
			if ((fontData << i) & 0x80) {
				PUTPIXEL((x + i), (y + j));
			}
		}
	}
	if (Zoom>1||bDelayDraw)
	{
		Redraw();
	}
	else
		BitBlt(hdcWindow, x+firstColumn, y+firstRow,
			lastColumn-firstColumn+1, lastRow-firstRow+1,
			hdcMemoryBuffer, x+firstColumn, y+firstRow, SRCCOPY);
	EndDraw();
}

/* ====================================================================== */
/*                                                                        */
/*   DrawVariableCharacter                                                */
/*                                                                        */
/* ====================================================================== */

void DrawVariableCharacter(unsigned char ch, short *x, short *y, short scale)
{
	unsigned short tempX;
	unsigned short tempY;
	unsigned short *p;
	unsigned short dydx;
	short dx;
	short dy;

	tempX = DrawingPositionX;
	tempY = DrawingPositionY;
	DrawingPositionX = *x;
	DrawingPositionY = *y;

	if ((CharSetSpecial == false) && (ch & 0x80)) {
		dx = scale * 8;
		DrawingPositionY -= dx;
		DrawSolidLineRelative(dx, 0);
		DrawingPositionX -= dx;
		DrawingPositionY += dx;

		/*
		 * the reference implementation puts this
		 * statement outside this block, which is
		 * likely to be a bug
		 */
		ch &= 0x7f;
	}

	if (CharSetIEEE == true) {
		if (ch < 0x20) {
			p = VectorFontIEEE[ch];
		} else {
			p = VectorFont[ch];
		}

		CharSetIEEE = false;
	} else {
		p = VectorFont[ch];
	}

	while (*p) {
		dydx = *p;

		dx = (dydx & 0x00ff);
		if (dx & 0x0080) {
			dx |= 0xff00;
		}

		dy = ((dydx >> 8) & 0x007f);
		if (dy & 0x0040) {
			dy |= 0xff80;
		}

		if (scale != 1) {
			dx *= scale;
			dy *= scale;
		}

		if ((dydx & 0x8000) != 0) {
			DrawSolidLineRelative(dx, dy);
		} else {
			DrawingPositionX += dx;
			DrawingPositionY += dy;
		}

		p++;
	}

	*x = DrawingPositionX;
	*y = DrawingPositionY;
	DrawingPositionX = tempX;
	DrawingPositionY = tempY;
}

/* -- END --------------------------------------------------------------- */
