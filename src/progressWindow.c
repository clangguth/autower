/*
    This file is part of autower, Copyright (C) Christoph Langguth, 2006 - 2010

    autower is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    autower is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with autower; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "display.h"
#include "progressWindow.h"
#include <commctrl.h>
#include <string.h>
#include <stdio.h>

HWND hwndWindow = 0;
HWND hwndProgressBar = 0;
HWND hwndFileCount = 0;
HWND hwndAirportCount = 0;
HWND hwndStatus = 0;
int totalLayers = 0;
int layersDone = 0;
int files = 0;
int airports = 0;

LRESULT CALLBACK progressWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int cxChar, cyChar;
	switch(message) {
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		InitCommonControls();

		hwndStatus = CreateWindow(
			"Static",
			"... indexing airports and towers, please hold the line ...",
			WS_CHILD | WS_VISIBLE ,
			0,
			0,
			50 * cxChar,
			cyChar,
			hwnd,
			NULL,
			NULL,
			NULL
		);

		hwndFileCount = CreateWindow(
			"Static",
			"Files scanned: 0",
			WS_CHILD | WS_VISIBLE ,
			0,
			2 * cyChar,
			50 * cxChar,
			cyChar,
			hwnd,
			NULL,
			NULL,
			NULL
		);

		hwndAirportCount = CreateWindow(
			"Static",
			"Airports found: 0",
			WS_CHILD | WS_VISIBLE ,
			0,
			3 * cyChar,
			50 * cxChar,
			cyChar,
			hwnd,
			NULL,
			NULL,
			NULL
		);

		hwndProgressBar = CreateWindowEx(
			0,
			PROGRESS_CLASS,
			"",
			WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
			0,
			4 * cyChar,
			50 * cxChar,
			cyChar,
			hwnd,
			NULL,
			NULL,
			NULL
		);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


DWORD WINAPI createWindow(LPVOID dummy) {
	int cxChar = LOWORD(GetDialogBaseUnits());
	int cyChar = HIWORD(GetDialogBaseUnits());
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = progressWindowProcedure;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	wndclass.lpszClassName = WINDOW_TITLE;
	wndclass.lpszMenuName = NULL;

	RegisterClass(&wndclass);

	hwndWindow = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_NOPARENTNOTIFY | WS_EX_TOOLWINDOW,
		WINDOW_TITLE,
		WINDOW_TITLE,
		WS_OVERLAPPED | WS_SIZEBOX,
		0,
		0,
		cxChar*55,
		cyChar*7,
		NULL,
		NULL,
		NULL,
		NULL
	);

	ShowWindow(hwndWindow, SW_SHOWDEFAULT);
	SetForegroundWindow(hwndWindow);
	UpdateWindow(hwndWindow);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.lParam;
}

void progressCreateWindow() {
	/* create the progress window in a new thread */
	DWORD dummy;
	CreateThread(NULL,0,&createWindow,NULL,0,&dummy);
}

void updateStats(BOOL force) {
	static DWORD lastTickCount = 0;
	DWORD newTickCount = GetTickCount();
	if (force || newTickCount >= lastTickCount + 10) {
		char buf[256];
		sprintf(buf, "Files scanned: %d (%d/%d scenery layers)", files, layersDone, totalLayers);
		SendMessage(hwndFileCount, WM_SETTEXT, 0, (LPARAM)buf);
		sprintf(buf, "Airports found: %d", airports);
		SendMessage(hwndAirportCount, WM_SETTEXT, 0, (LPARAM)buf);
		lastTickCount = newTickCount;
	}
}
void progressIncrementFileCount(int inc) {
	files += inc;
	updateStats(FALSE);
}

void progressIncrementAirportCount(int inc) {
	airports += inc;
	updateStats(FALSE);
}

DWORD timing = 0;

void progressSetTotalLayersCount(int layersCount) {
	int i;
	totalLayers = layersCount;
	/* since the window is created in a new thread, it might not yet be fully constructed
	 * at the time this function is first called
	 */
	for (i=0; i < 100; i++) {
		if (hwndProgressBar != 0) break;
		Sleep(50);
	}
	SendMessage(hwndProgressBar, PBM_SETRANGE, 0, MAKELPARAM (0, totalLayers));
	SendMessage(hwndProgressBar, PBM_SETSTEP, (WPARAM) 1, 0);
	timing = GetTickCount();
}

void progressLayerDone() {
	layersDone++;
	SendMessage(hwndProgressBar, PBM_STEPIT, 0, 0);
}

void progressFinalize() {
	timing = GetTickCount() - timing;
	updateStats(TRUE);
	char buf[256];
	sprintf(buf, "Airport Database created in %u ms", (unsigned int) timing);
	SendMessage(hwndStatus, WM_SETTEXT, 0, (LPARAM)buf);
	SetForegroundWindow(hwndWindow);
	// close the progress window after some time.
	Sleep(2500);
	SendMessage(hwndWindow, WM_DESTROY, 0, 0);
#ifdef BUILD_DLL
	HWND hwndFSMain = FindWindow("FS98MAIN",NULL);
	ShowWindow(hwndFSMain, SW_RESTORE);
#endif
}
