/*
    This file is part of autower, Copyright (C) Christoph Langguth

    autower is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2,
    as published by the Free Software Foundation.

    autower is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License, version 2, for more details. A copy of
    the license is included with this distribution (LICENSE.txt).
*/

#include <windows.h>
#include <stdio.h>
#include "display.h"
#ifdef BUILD_DLL
#include "fsui.h"
#endif

int displayLevel;

void setDisplayLevel(int level) {
	displayLevel = level;
}

void displayCombined(LPSTR message, int level) {
#ifdef BUILD_DLL
	if (level <= DISPLAY_ERROR)
		FSMessageBox(message,MB_ICONERROR);
	else if (level == DISPLAY_WARN)
		FSMessageBox(message,MB_ICONWARNING);
	else
		FSMessageBox(message,MB_ICONINFORMATION);
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

