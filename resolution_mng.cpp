/*
MIT License

Copyright (c) 2017 Naohisa Fukuoka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <winsock2.h>
#include <ws2bth.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#define DISPLAY_APP_NAME "resolution_mng"
#define SET_INTERVAL_MSEC	(5000)
WSAEVENT g_hEventExit = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
long SetResolution(int width, int height, int monitor);

DWORD WINAPI ThreadProcUnity(LPVOID lpParamater);

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int nCmdShow)
{
	TCHAR      szAppName[] = TEXT(DISPLAY_APP_NAME);
	HWND       hwnd;
	MSG        msg;
	WNDCLASSEX wc;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinst;
	wc.hIcon         = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	wc.hCursor       = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szAppName;
	wc.hIconSm       = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
	
	if (RegisterClassEx(&wc) == 0)
		return 0;

	hwnd = CreateWindowEx(0, szAppName, szAppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 340, 40, NULL, NULL, hinst, NULL);
	if (hwnd == NULL)
		return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND   hwndButton = NULL;
	static HWND   hwndEdit = NULL;
	static HWND   hwndState = NULL;
	static HWND   hwndGuide = NULL;
	static HWND   hwndMemo = NULL;
	static HWND   hwndList = NULL;
	static HANDLE hThread = NULL;
	static SOCKET soc = INVALID_SOCKET;
	static HFONT hFont1;

	static int timerCounter = 0;
	static int sockServerEnable = 0;

	switch (uMsg) {

	case WM_CREATE: {
		SetTimer(hwnd, 0, SET_INTERVAL_MSEC, NULL);
		return 0;
	}
	case WM_COMMAND: {
		return 0;
	}
	case WM_TIMER: {
		(void)SetResolution(1920, 1080, 0);
		SetTimer(hwnd, 0, SET_INTERVAL_MSEC, NULL);
		return 0;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

long SetResolution(int width, int height, int monitor)
{
	// get device
	DISPLAY_DEVICE disp = { 0 };
	disp.cb = sizeof(disp);
	EnumDisplayDevices(NULL, monitor, &disp, NULL);

	// get resolution
	DEVMODE curr = { 0 };
	curr.dmSize = sizeof(curr);
	EnumDisplaySettings(disp.DeviceName, ENUM_CURRENT_SETTINGS, &curr);

	if (curr.dmPelsWidth == width && curr.dmPelsHeight == height) {
		return(0);
	}

	// set resolution
	DEVMODE mode = { 0 };
	mode.dmSize = sizeof(mode);
	mode.dmPelsWidth = width;
	mode.dmPelsHeight = height;
	mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	return(ChangeDisplaySettingsEx(disp.DeviceName, &mode, NULL, 0, NULL));
}
