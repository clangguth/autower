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

#include <windows.h>
#include "display.h"
#include <stdio.h>

int displayLevel;

void setDisplayLevel(int level) {
	displayLevel = level;
}

void displayCombined(LPSTR message, int level) {
#ifdef BUILD_DLL
	if (level <= DISPLAY_ERROR)
		MessageBox(NULL,message,WINDOW_TITLE,MB_ICONERROR);
	else if (level == DISPLAY_WARN)
		MessageBox(NULL,message,WINDOW_TITLE,MB_ICONWARNING);
	else
		MessageBox(NULL,message,WINDOW_TITLE,MB_ICONINFORMATION);
#else
	printf(message);
	if (message[strlen(message)-1] != '\n') printf("\r\n");
#endif
}

void display(int level, const char *fmt, ...) {
	va_list arg;
	char buf[2048];
	va_start(arg, fmt);
	if (level <= displayLevel) {
		vsprintf(buf, fmt, arg);
		displayCombined(buf, level);
	}
	va_end(arg);
}

LPVOID getFormattedErrorMessage() {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	return lpMsgBuf;
}

