/* ********************************************************************** */
/*                                                                        */
/*   VIDEO.C                                                              */
/*                                                                        */
/* ********************************************************************** */

/* -- HEADER FILES ------------------------------------------------------ */

#include "video.h"
#include <stdio.h>

#define IDLETIMER 1

#ifndef DEF_IDLETIMER
#define DEF_IDLETIMER   4
#endif

/* -- PUBLIC DATA DEFINITIONS ------------------------------------------- */

HWND hWnd;
HDC hdcWindow;
HANDLE hmMemoryBuffer;
HDC hdcMemoryBuffer;
HDC hdcScaledBuffer;
HANDLE hKeyMouseEvent;

BOOL bDelayDraw = FALSE;
int RedrawTimer = 2;
int IdleTimer = 10;
int nDisplayCount = 0;

/* -- PRIVATE DATA DEFINITIONS ------------------------------------------ */

static HANDLE hThread;
static DWORD dwThreadID;
static HBITMAP hbmWindow;
static HBITMAP hbmScaled;
static HANDLE hCreateDone;

static BYTE KeyState = 0;
static int MouseXPos, MouseYPos;
static int MouseXCnt, MouseYCnt, MouseXCntMax, MouseYCntMax;
static WORD ButtonStat = 0;
static BOOL bCapture = FALSE;
static BOOL bCenter = FALSE;
static int MouseWheelCount = 0;

static unsigned short FuncKeyNormScanCode[] = {0x3B00,0x3C00,0x3D00,0x3E00,
				0x3F00,0x4000,0x4100,0x4200,0x4300,0x4400,0x8500,0x8600};
static unsigned short FuncKeyShftScanCode[] = {0x5400,0x5500,0x5600,0x5700,
				0x5800,0x5900,0x5A00,0x5B00,0x5C00,0x5D00,0x8700,0x8800};
static unsigned short FuncKeyCtrlScanCode[] = {0x5E00,0x5F00,0x6000,0x6100,
				0x6200,0x6300,0x6400,0x6500,0x6600,0x6700,0x8900,0x8A00};
static unsigned short FuncKeyAltScanCode[] = {0x6800,0x6900,0x6A00,0x6B00,
				0x6C00,0x6D00,0x6E00,0x6F00,0x7000,0x7100,0x8B00,0x8C00};

static unsigned short NumPadNormScanCode[] = {0x4900,0x5100,0x4F00,0x4700,
				0x4B00,0x4800,0x4D00,0x5000};
static unsigned short NumPadShftScanCode[] = {0x4939,0x5133,0x4F31,0x4737,
				0x4B34,0x4838,0x4D36,0x5032};
static unsigned short NumPadCtrlScanCode[] = {0x8400,0x7600,0x7500,0x7700,
				0x7300,0x8D00,0x7400,0x9100};
static unsigned short NumPadAltScanCode[] = {0x9900,0xA100,0x9F00,0x9700,
				0x9B00,0x9800,0x9D00,0xA000};

static unsigned short NumKeyAltScanCode[] = {0x8100, 0x7800, 0x7900, 0x7A00, 0x7B00,
				0x7C00, 0x7D00, 0x7E00, 0x7F00, 0x8000};

static unsigned short AlphaAltScanCode[] = {0x1E00,0x3000,0x2E00,0x2000,0x1200,0x2100,
				0x2200,0x2300,0x1700,0x2400,0x2500,0x2600,0x3200,0x3100,0x1800,0x1900,
				0x1000,0x1300,0x1F00,0x1400,0x1600,0x2F00,0x1100,0x2D00,0x1500,0x2C00};

/* -- PRIVATE FUNCTION PROTOTYPES --------------------------------------- */

DWORD CALLBACK WndMsgLoop(LPVOID lpvParameter);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void OnSysKeyDown(WPARAM wParam, LPARAM lParam);
void OnKeyDown(WPARAM wParam, LPARAM lParam);
BYTE GetShiftState(void);
unsigned short GetNumPadScanCode(int wParam);
unsigned short GetFunKeyScanCode(int wParam);
BOOL GetMouseCount(void);
void StartTimer(void);
void ZoomToggle(void);

static unsigned short ScanCode = 0;
static unsigned short LastCode = 0;
static char s[20];

/* -- Used for multi-montor selection ----------------------------------- */

HMONITOR hMonitor2Use;
RECT rectMonitor;

/* ---------------------------------------------------------------------- */

#define SHIFTED 0x8000
#define TOGGLED 0x0001

/* -- CODE -------------------------------------------------------------- */

/* ====================================================================== */
/*                                                                        */
/*   InitVideo                                                            */
/*                                                                        */
/* ====================================================================== */

bool InitVideo(unsigned short resolutionX, unsigned short resolutionY)
{
	WNDCLASS wc;

	ResolutionX = resolutionX;
	ResolutionY = resolutionY;

	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindow";

	if (RegisterClass(&wc) == 0) {
		return false;
	}

	hKeyMouseEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	hCreateDone = CreateEvent(NULL, TRUE, FALSE, NULL);

	hThread = CreateThread(NULL, 0, WndMsgLoop, 0, 0,
		&dwThreadID);

	if (hThread)
		WaitForSingleObject(hCreateDone, INFINITE);

	CloseHandle(hCreateDone);

	if (hWnd==0)
		return false;

	ResolutionMaxXY = ResolutionX;
	if (ResolutionX<ResolutionY)
		ResolutionMaxXY = ResolutionY;

	return true;
}

/* ====================================================================== */
/*                                                                        */
/*   ShutdownVideo                                                        */
/*                                                                        */
/* ====================================================================== */

void ShutdownVideo()
{
	PostMessage(hWnd, WM_CLOSE, 0, 0);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hmMemoryBuffer);
	CloseHandle(hKeyMouseEvent);

	UnregisterClass("MainWindow", hInstance);
}

/* ====================================================================== */
/*                                                                        */
/*   WndMsgLoop                                                           */
/*                                                                        */
/* ====================================================================== */

DWORD CALLBACK WndMsgLoop(LPVOID lpvParameter)
{
	MSG msg;

	unsigned short resX, resY;
	RECT rcClient, rcWindow;
	POINT ptDiff;
	HWND hDosWnd, hOldWnd;
	DWORD dwDosThreadId;
	int i;

	DWORD dwStyle = WS_CAPTION | WS_VISIBLE;

	hMonitor2Use = NULL;

	rectMonitor.left = 0;
	rectMonitor.top = 0;
	rectMonitor.right = 0;
	rectMonitor.bottom = 0;

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	if (ResolutionX)
		resX = ResolutionX;
	else
	if (hMonitor2Use!=NULL)
		resX = rectMonitor.right-rectMonitor.left;
	else
		resX = GetSystemMetrics(SM_CXSCREEN);

	if (ResolutionY)
		resY = ResolutionY;
	else
	if (hMonitor2Use!=NULL)
		resY = rectMonitor.bottom-rectMonitor.top;
	else
		resY = GetSystemMetrics(SM_CYSCREEN);

	if (((resX==GetSystemMetrics(SM_CXSCREEN))
	 &&(resY==GetSystemMetrics(SM_CYSCREEN)))
	 ||((resX==(rectMonitor.right-rectMonitor.left))
	 &&(resY==(rectMonitor.bottom-rectMonitor.top))))
		dwStyle = WS_POPUP;

	hWnd = CreateWindow("MainWindow", "OrCAD GDI Display", dwStyle,
		0, 0, resX, resY, NULL, NULL, hInstance, NULL);

	if (hWnd == 0) {
		SetEvent(hCreateDone); // Signal Windows creation is done.
		return -1;
	}

	hdcWindow = GetDC(hWnd);
	if ((GetDeviceCaps(hdcWindow, RASTERCAPS)&RC_BITBLT)==0)
	{
		MessageBox(NULL, "Video Card is unsupported.", NULL, MB_OK);
		DestroyWindow(hWnd);
		hWnd = 0;
		SetEvent(hCreateDone); // Signal Windows creation is done.
		return -1;
	}

	if (dwStyle!=WS_POPUP)
	{
		GetClientRect(hWnd, &rcClient);
		GetWindowRect(hWnd, &rcWindow);

		ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
		ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

		if ((ptDiff.x>0)||(ptDiff.y>0))
		{
            rcWindow.left   = ((rectMonitor.right-rectMonitor.left)
                            - (ResolutionX + ptDiff.x))/2;
            rcWindow.top    = ((rectMonitor.bottom-rectMonitor.top)
                            - (ResolutionY + ptDiff.y))/2;

			MoveWindow(hWnd, rcWindow.left, rcWindow.top,
				ResolutionX + ptDiff.x, ResolutionY + ptDiff.y, TRUE);
		}
	}
	else
	if ((resX==rectMonitor.right-rectMonitor.left)
	 && (resY==rectMonitor.bottom-rectMonitor.top))
	{
		MoveWindow(hWnd, rectMonitor.left, rectMonitor.top,
			resX, resY, FALSE);
	}

	/* Confirm resolutions after adjustment */
	GetClientRect(hWnd, &rcClient);
	ResolutionX = rcClient.right;
	ResolutionY = rcClient.bottom;

	hdcMemoryBuffer = CreateCompatibleDC(hdcWindow);
	hbmWindow = CreateCompatibleBitmap(hdcWindow,
		ResolutionX, ResolutionY);

	hdcScaledBuffer = CreateCompatibleDC(hdcWindow);
	hbmScaled = CreateCompatibleBitmap(hdcWindow,
		ResolutionX, ResolutionY);

	hDosWnd = GetForegroundWindow();
	dwDosThreadId = GetWindowThreadProcessId(hDosWnd,NULL);

	AttachThreadInput(dwDosThreadId, GetCurrentThreadId(), TRUE);

	for (i=0, hOldWnd=hDosWnd; (i<10)&&(hOldWnd!=hWnd); i++)
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);

		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		ShowWindow(hDosWnd, SW_HIDE);

		hOldWnd = GetForegroundWindow();
	}

	SelectObject(hdcMemoryBuffer, hbmWindow);
	SelectObject(hdcScaledBuffer, hbmScaled);

	InitCursor();

	hmMemoryBuffer = CreateMutex(NULL, false, "hdcMemoryBuffer");

	MouseXCnt = 0;
	MouseYCnt = 0;
	ButtonStat = 0;
	MouseXCntMax = ResolutionX/4;
	MouseYCntMax = ResolutionY/4;

//	if (dwStyle==WS_POPUP)
	{
		bCapture = TRUE;
		bCenter = TRUE;
		GetMouseCount();
		MouseXCnt = 0;
		MouseYCnt = 0;
		ButtonStat = 0;
		for (i=0; i<20 && nDisplayCount>=0; i++)
			nDisplayCount = ShowCursor(FALSE);
	}

	StartTimer();

	SetEvent(hCreateDone); // Signal Windows creation is done.

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ShowWindow(hDosWnd, SW_SHOW);
	ShowWindow(hDosWnd, SW_RESTORE);
	SetForegroundWindow(hDosWnd);
	SetFocus(hDosWnd);

	DestroyWindow(hWnd);

	ReleaseDC(hWnd, hdcWindow);
	DeleteDC(hdcMemoryBuffer);
	DeleteObject(hbmWindow);

	if (hdcSavedRect)
	{
		DeleteDC(hdcSavedRect);
		DeleteObject(hbmSavedRect);
	}

	UnloadCursor();

	AttachThreadInput(dwDosThreadId, GetCurrentThreadId(), FALSE);

	return msg.wParam;
}

/* ====================================================================== */
/*                                                                        */
/*   WndProc                                                              */
/*                                                                        */
/* ====================================================================== */

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT Ps;
	HDC hDC;
    int i;

	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			WaitForSingleObject(hmMemoryBuffer, INFINITE);
			hDC = BeginPaint(hWnd, &Ps);
			if (Zoom>1)
			{
				StretchBlt(hdcScaledBuffer, 0, 0, ResolutionX, ResolutionY,
					hdcMemoryBuffer, ZoomScrollX, ZoomScrollY,
					ResolutionX/2, ResolutionY/2, SRCCOPY);
				BitBlt(hDC, 0, 0, ResolutionX, ResolutionY,
					hdcScaledBuffer, 0, 0, SRCCOPY);
			}
			else
			{
				BitBlt(hDC, 0, 0, ResolutionX, ResolutionY,
					hdcMemoryBuffer, 0, 0, SRCCOPY);
			}
			EndPaint(hWnd, &Ps);
			ReleaseMutex(hmMemoryBuffer);
			break;
		case WM_MOUSEMOVE:
			if (bCapture&&!bCenter)
				SetEvent(hKeyMouseEvent);
			break;
		case WM_LBUTTONDOWN:
			if (!bCapture)
			{
				bCapture = TRUE;
				bCenter = TRUE;
				GetMouseCount();
				MouseXCnt = 0;
				MouseYCnt = 0;
				ButtonStat = 0;
		        for (i=0; i<20 && nDisplayCount>=0; i++)
					nDisplayCount = ShowCursor(FALSE);
				KeyState = GetShiftState();
			}
			else
			{
				ButtonStat |= MK_LBUTTON;
				StartTimer();
			}
			SetEvent(hKeyMouseEvent);
			break;
		case WM_LBUTTONUP:
			ButtonStat &= ~MK_LBUTTON;
			break;
		case WM_RBUTTONDOWN:
			if (bCapture)
			{
				ButtonStat |= MK_RBUTTON;
				StartTimer();
				SetEvent(hKeyMouseEvent);
			}
			break;
		case WM_RBUTTONUP:
			ButtonStat &= ~MK_RBUTTON;
			break;
#if 0
		case WM_MOUSEWHEEL:
			MouseWheelCount += (int)wParam/65536L;
			if (MouseWheelCount>=240)
			{
				MouseWheelCount -= 240;
				if (Zoom==2)
				{
					Zoom = 1;
					Redraw();
				}
				else
				if (Zoom==1)
				{
					Zoom = 0;
					ScanCode = 0x6D00;  //ALT F6
				}
			}
			else
			if (MouseWheelCount<=-240)
			{
				MouseWheelCount += 240;
				if (Zoom==1&&Scale==1)
				{
					Zoom = 2;
					ZoomScrollX = 0;
					ZoomScrollY = 0;
					ZoomScroll();
					Redraw();
				}
				else
					ScanCode = 0x6C00;  //ALT F5
			}
			break;
#endif
		case WM_KILLFOCUS:
			if (bCapture)
			{
		        for (i=0; i<20 && nDisplayCount<0; i++)
					nDisplayCount = ShowCursor(TRUE);
				bCapture = FALSE;
				ButtonStat = 0;
			}
			break;
		case WM_CHAR:
			ScanCode = (unsigned short)((0xFF&wParam)+((0xFF0000&lParam)>>8));
			StartTimer();
			break;
		case WM_SYSKEYDOWN:
			OnSysKeyDown(wParam, lParam);
			break;
		case WM_KEYUP:
			KeyState = GetShiftState();
			break;
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			break;
		case WM_TIMER:
			if (IdleTimer>0)
				IdleTimer--;
			else
			{
				ResetEvent(hKeyMouseEvent);
				IdleTimer = DEF_IDLETIMER;
			}
			if (RedrawTimer>0)
			{
				RedrawTimer--;
				if (RedrawTimer==0)
				{
					WaitForSingleObject(hmMemoryBuffer, INFINITE);
					if (Zoom>1)
					{
						StretchBlt(hdcScaledBuffer, 0, 0, ResolutionX, ResolutionY,
							hdcMemoryBuffer, ZoomScrollX, ZoomScrollY,
							ResolutionX/2, ResolutionY/2, SRCCOPY);
						BitBlt(hdcWindow, 0, 0, ResolutionX, ResolutionY,
							hdcScaledBuffer, 0, 0, SRCCOPY);
					}
					else
					{
						BitBlt(hdcWindow, 0, 0, ResolutionX, ResolutionY,
							hdcMemoryBuffer, 0, 0, SRCCOPY);
					}
					ReleaseMutex(hmMemoryBuffer);
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

/* ====================================================================== */

void OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
    int i;
#if _DEBUG_
	char s[80];
#endif

	StartTimer();
	switch (wParam) {
	case VK_RETURN:
	case VK_ESCAPE:
        for (i=0; i<20 && nDisplayCount<0; i++)
			nDisplayCount = ShowCursor(TRUE);
		bCapture = FALSE;
		ButtonStat = 0;
		break;
	case VK_MULTIPLY:
		ScanCode = 0x3700;
		break;
	case VK_ADD:
		ScanCode = 0x4E00;
		break;
	case VK_SUBTRACT:
		ScanCode = 0x4A00;
		break;
	case VK_DIVIDE:
		ScanCode = 0xA400;
		break;
	case VK_F1:
	case VK_F2:
	case VK_F3:
	case VK_F4:
	case VK_F5:
	case VK_F6:
	case VK_F7:
	case VK_F8:
	case VK_F9:
	case VK_F11:
	case VK_F12:
		ScanCode = FuncKeyAltScanCode[wParam-VK_F1];
		break;
	case VK_F10:
		KeyState = GetShiftState();
		if (GetAsyncKeyState(VK_MENU)&SHIFTED)
			ScanCode = 0x7100;
		else
		if (GetAsyncKeyState(VK_CONTROL)&SHIFTED)
			ScanCode = 0x6700;
		else
		if (GetAsyncKeyState(VK_SHIFT)&SHIFTED)
			ScanCode = 0x5D00;
		else
			ScanCode = 0x4400;
		break;
	case VK_PRIOR:  // Num 9
	case VK_NEXT:   // Num 3
	case VK_END:    // Num 1
	case VK_HOME:   // Num 7
	case VK_LEFT:   // Num 4
	case VK_UP:     // Num 8
	case VK_RIGHT:  // Num 6
	case VK_DOWN:   // Num 2
		ScanCode = NumPadAltScanCode[wParam-VK_PRIOR];
		break;
	case VK_INSERT:
		ScanCode = 0xA200;
		break;
	case VK_DELETE:
		ScanCode = 0xA300;
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		ScanCode = NumKeyAltScanCode[wParam-'0'];
		break;
	case VK_OEM_MINUS:
		ScanCode = 0x8200;
		break;
	case VK_OEM_PLUS:
		ScanCode = 0x8300;
		break;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		ScanCode = AlphaAltScanCode[wParam-'A'];
		break;
	}
}

/* ====================================================================== */

void OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	KeyState = GetShiftState();
	switch (wParam) {
	case VK_PRIOR:  // Num 9
	case VK_NEXT:   // Num 3
	case VK_END:    // Num 1
	case VK_HOME:   // Num 7
	case VK_LEFT:   // Num 4
	case VK_UP:     // Num 8
	case VK_RIGHT:  // Num 6
	case VK_DOWN:   // Num 2
		ScanCode = GetNumPadScanCode(wParam);
		StartTimer();
		break;
	case VK_INSERT: //
		if (KeyState&0x04)
			ScanCode = 0x9200;
		else
		if (KeyState&0x03)
			ScanCode = 0x5230;
		else
			ScanCode = 0x5200;
		StartTimer();
		break;
	case VK_DELETE:
		if (KeyState&0x04)
			ScanCode = 0x9300;
		else
		if (KeyState&0x03)
			ScanCode = 0x532E;
		else
			ScanCode = 0x5300;
		StartTimer();
		break;
	case VK_F1:
	case VK_F2:
	case VK_F3:
	case VK_F4:
	case VK_F5:
	case VK_F6:
	case VK_F7:
	case VK_F8:
	case VK_F9:
	case VK_F10:
	case VK_F11:
	case VK_F12:
		ScanCode = GetFunKeyScanCode(wParam);
		StartTimer();
		break;
	case '1':
		if (KeyState&0x04)
		{
			ZoomToggle();
		}
		break;
	case '3':
		if (KeyState&0x04)
		{
			bDelayDraw = !bDelayDraw;
		}
		break;
	}
}

/* ====================================================================== */

BYTE GetShiftState(void)
{
	BYTE State = 0;
	State += (GetAsyncKeyState(VK_RSHIFT)&SHIFTED?1:0);
	State += (GetAsyncKeyState(VK_LSHIFT)&SHIFTED?2:0);
	State += (GetAsyncKeyState(VK_CONTROL)&SHIFTED?4:0);
	State += (GetAsyncKeyState(VK_MENU)&SHIFTED?8:0);
	State += (GetAsyncKeyState(VK_SCROLL)&TOGGLED?0x10:0);
	State += (GetAsyncKeyState(VK_NUMLOCK)&TOGGLED?0x20:0);
	State += (GetAsyncKeyState(VK_CAPITAL)&TOGGLED?0x40:0);
	State += (GetAsyncKeyState(VK_INSERT)&TOGGLED?0x80:0);
	return State;
}

/* ====================================================================== */

unsigned short GetNumPadScanCode(int wParam)
{
	int index = wParam-VK_PRIOR;
	if (KeyState&0x04)
	{
		return NumPadCtrlScanCode[index];
	}
	else
	if (KeyState&0x03)
	{
		return NumPadShftScanCode[index];
	}
	else
		return NumPadNormScanCode[index];
}

/* ====================================================================== */

unsigned short GetFunKeyScanCode(int wParam)
{
	int index = wParam-VK_F1;
/*
	if (KeyState&0x08)
	{
		MessageBox(NULL, NULL, "Alt Key Fx", MB_OK);
		return FuncKeyAltScanCode[index];
	}
	else
*/
	if (KeyState&0x04)
	{
		return FuncKeyCtrlScanCode[index];
	}
	else
	if (KeyState&0x03)
	{
		return FuncKeyShftScanCode[index];
	}
	else
		return FuncKeyNormScanCode[index];
}

/* ====================================================================== */
/*                                                                        */
/*   Translate                                                            */
/*                                                                        */
/* ====================================================================== */

void Translate(short *const x, short *const y)
{
	*x -= WindowStartX;
	*y -= WindowStartY;

	if (Scale > 1) {
		*x /= Scale;
		*y /= Scale;
	}

	*x += WindowOriginX;
	*y += WindowOriginY;
}

/* ====================================================================== */
/*                                                                        */
/*   PutPixel                                                             */
/*                                                                        */
/* ====================================================================== */

void PutPixel(short x, short y)
{
	SetPixel(hdcMemoryBuffer, x, y, RGBPalette[MapMask]);
	if (Zoom>1)
	{
		ZoomXY(x, y);
		if (x>=0 && x<(ResolutionX-1) && y>=0 && y<(ResolutionY-1))
		{
			SetPixel(hdcWindow, x, y, RGBPalette[MapMask]);
			SetPixel(hdcWindow, x+1, y, RGBPalette[MapMask]);
			SetPixel(hdcWindow, x, y+1, RGBPalette[MapMask]);
			SetPixel(hdcWindow, x+1, y+1, RGBPalette[MapMask]);
		}
	}
	else
		SetPixel(hdcWindow, x, y, RGBPalette[MapMask]);
}

/* ====================================================================== */
/*                                                                        */
/*   KeybrdServe                                                          */
/*                                                                        */
/* ====================================================================== */

void KeybrdServe(union REGPACK *pRegs)
{
	switch (pRegs->h.ah) {
	case 0:
	case 0x10:
		if (ScanCode&&bCapture)
        {
			pRegs->w.flags &= ~INTR_ZF;
			pRegs->w.ax = ScanCode;
			LastCode = ScanCode;
			ScanCode = 0;
		}
		else
		{
			ScanCode = 0;
			WaitForSingleObject(hKeyMouseEvent, 100);
			pRegs->w.flags |= INTR_ZF;
			pRegs->w.ax = LastCode;
		}
		break;
	case 1:
	case 0x11:
		/* Slow down to save on CPU cycles */
		if (ScanCode&&bCapture)
		{
			pRegs->w.ax = ScanCode;
			pRegs->w.flags &= ~INTR_ZF;
		}
		else
		{
			ScanCode = 0;
			if (IdleCounter!=0)
				IdleCounter--;
			else
			{
				WaitForSingleObject(hKeyMouseEvent, 20);
				IdleCounter = 10;
			}
			pRegs->w.ax = LastCode;
			pRegs->w.flags |= INTR_ZF;
		}
		break;
	case 2: // Get shift status
		pRegs->h.al = KeyState;
		break;
	default:
		pRegs->w.ax = 0;
		pRegs->w.flags |= INTR_ZF;
	}
}

/* ====================================================================== */
/*                                                                        */
/*   MouseServe                                                           */
/*                                                                        */
/* ====================================================================== */

void MouseServe(union REGPACK *pRegs)
{
	switch (pRegs->w.ax) {
	case 0: // InitMouse
		pRegs->w.ax = 0xFFFF;
		pRegs->w.bx = 2;
		MouseXCnt = 0;
		MouseYCnt = 0;
		break;
	case 1: // Show cursor
		MessageBox(NULL, NULL, "Show Cursor", MB_OK);
		break;
	case 2: // Hide cursor
		MessageBox(NULL, NULL, "Hide Cursor", MB_OK);
		break;
	case 3:
		pRegs->w.bx = ButtonStat;
		pRegs->w.cx = (640*MouseXPos)/ResolutionX;
		pRegs->w.dx = (200*MouseYPos)/ResolutionY;
		break;
	case 4:
		sprintf(s, "%3i, %3i", pRegs->w.cx, pRegs->w.dx);
		MessageBox(NULL, s, "Set Mouse Position", MB_OK);
		break;
	case 0xB:
		if (GetMouseCount())
		{
			SetEvent(hKeyMouseEvent);
		}
		pRegs->w.cx = MouseXCnt; MouseXCnt = 0;
		pRegs->w.dx = MouseYCnt; MouseYCnt = 0;
		break;
	default:
		sprintf(s, "%03X", pRegs->w.ax);
		MessageBox(NULL, s, "Misc. Mouse Command", MB_OK);
	}
}

/* ====================================================================== */
/*                                                                        */
/*   CenterCursor                                                         */
/*                                                                        */
/* ====================================================================== */
void CenterCursor(void)
{
	RECT rcWindow;
	int X, Y;
	POINT point;

	GetWindowRect(hWnd, &rcWindow);
	X = (rcWindow.left + rcWindow.right)/2;
	Y = (rcWindow.top + rcWindow.bottom)/2;

	if (SetCursorPos(X,Y))
	{
		bCenter = TRUE;

		if (GetCursorPos(&point))
		{
			MouseXPos = point.x;
			MouseYPos = point.y;
		}
		else
		{
			MouseXPos = X;
			MouseYPos = Y;
		}
	}
}

/* ====================================================================== */
/*                                                                        */
/*   GetMouseCount                                                        */
/*                                                                        */
/* ====================================================================== */

BOOL GetMouseCount(void)
{
	BOOL result;
	POINT point;

	result = FALSE;

	if (GetCursorPos(&point))
	{
		if (bCapture)
		{
			MouseXCnt = point.x - MouseXPos;
			MouseYCnt = point.y - MouseYPos;

			MouseXPos = point.x;
			MouseYPos = point.y;

			if (bCenter||
				(abs(MouseXCnt)>MouseXCntMax)||
				(abs(MouseYCnt)>MouseYCntMax))
			{
				bCenter = FALSE;

				MouseXCnt = 0;
				MouseYCnt = 0;
			}
			else
			if (MouseXCnt||MouseYCnt)
			{
				MouseXPos = point.x;
				MouseYPos = point.y;

				CenterCursor();

				result = TRUE;
			}
		}
		else
		{
			MouseXPos = point.x;
			MouseYPos = point.y;
		}
	}

	return result;
}

/* ====================================================================== */
/*                                                                        */
/*   StartTimer                                                           */
/*                                                                        */
/* ====================================================================== */

void StartTimer(void)
{
	IdleTimer = DEF_IDLETIMER;
	SetTimer(hWnd,IDLETIMER,10,NULL);
	SetEvent(hKeyMouseEvent);
}

/* ====================================================================== */
/*                                                                        */
/*   MonitorEnumProc                                                      */
/*                                                                        */
/* ====================================================================== */

BOOL CALLBACK MonitorEnumProc(
  HMONITOR hMonitor,  // handle to display monitor
  HDC hdcMonitor,     // handle to monitor DC
  LPRECT lprcMonitor, // monitor intersection rectangle
  LPARAM dwData       // data
)
{
	int SizeX, SizeY, ResX, ResY;

	ResX = rectMonitor.right - rectMonitor.left;
	ResY = rectMonitor.bottom - rectMonitor.top;

	SizeX = lprcMonitor->right - lprcMonitor->left;
	SizeY = lprcMonitor->bottom - lprcMonitor->top;

	if ((SizeX*SizeY)>(ResX*ResY))
	{
		hMonitor2Use = hMonitor;
		rectMonitor.top = lprcMonitor->top;
		rectMonitor.bottom = lprcMonitor->bottom;
		rectMonitor.left = lprcMonitor->left;
		rectMonitor.right = lprcMonitor->right;
	}

	return TRUE;
}

/* ====================================================================== */
/*                                                                        */
/*   VideoInt10Serve                                                      */
/*                                                                        */
/* ====================================================================== */

void VideoInt10Serve(union REGPACK *pRegs)
{
	if (pRegs->w.ax==0x0E07)
	{
		Beep(1000,150);
	}
#if 0
	else
	{
		sprintf(s, "%04X, %04X", pRegs->w.ax, pRegs->w.bx);
		MessageBox(NULL, s, "VideoInt10Serve", MB_OK);
	}
#endif
}

/* ====================================================================== */
/*                                                                        */
/*   ZoomToggle                                                           */
/*                                                                        */
/* ====================================================================== */

void ZoomToggle(void)
{
	if (Scale==1)
	{
		if (Zoom==2)
		{
			Zoom = 1;
			Redraw();
		}
		else
		if (Zoom==1)
		{
			Zoom = 2;
			ZoomScrollX = 0;
			ZoomScrollY = 0;
			ZoomScroll();
			Redraw();
		}
		else
		if (Zoom==0)
			Zoom = 1;
	}
	else
	{
		if (Zoom<3)
			Zoom = 1;
	}
}

/* -- END --------------------------------------------------------------- */
