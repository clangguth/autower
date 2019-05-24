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
#include <stdio.h>
#include "progresswindowFS.h"
#include "progresswindowBasic.h"
#include "linkage.h"
#include "config.h"
#include "product.h"

extern IMPORTTABLE ImportTable;
extern Configuration* config;

#define GET_ADDRESS 0x102
WINAPI LPVOID (*getAddress)(char* entry, char* module);
__cdecl HANDLE (*FSCreateModeless)(LPVOID unknown, UINT _100perc, LPVOID zero);
__cdecl void (*FSShowModeless)(HANDLE handle, BOOL show);
__cdecl void (*FSUpdateModeless)(HANDLE handle, UINT done);
__cdecl void (*FSUpdateModelessMessage)(HANDLE handle, char* message);
__cdecl void (*FSCloseModeless)(HANDLE handle);

BOOL setupOK = FALSE;
HANDLE handle = NULL;

void progressCreateWindowFS() {
	if (config->integratedUI) {
		getAddress = ImportTable.modules[IMPORT_FS9].functions[GET_ADDRESS];
		FSCreateModeless = getAddress("FSCreateModeless", "fsui.dll");
		setupOK = FSCreateModeless != NULL;
		FSShowModeless = getAddress("FSShowModeless", "fsui.dll");
		setupOK &= FSShowModeless != NULL;
		FSUpdateModeless = getAddress("FSUpdateModeless", "fsui.dll");
		setupOK &= FSUpdateModeless != NULL;
		FSUpdateModelessMessage = getAddress("FSUpdateModelessMessage", "fsui.dll");
		setupOK &= FSUpdateModelessMessage != NULL;
		FSCloseModeless = getAddress("FSCloseModeless", "fsui.dll");
		setupOK &= FSCloseModeless != NULL;
	} else {
		setupOK = FALSE;
	}

	if (!setupOK) {
		progressCreateWindowBasic();
		return;
	}
}

/* Yuck. This global namespace thing in C really sucks. */
int airports1 = 0;
int layers1 = 0;
int totalLayers1 = 0;
int files1 = 0;

void updateWindow(BOOL updateLayers) {
	static DWORD lastUpdate = 0;
	char buf[256];
	if (!updateLayers && GetTickCount() - lastUpdate <= 100) {
		return;
	}
	lastUpdate = GetTickCount();
	sprintf(buf, PRODUCT " is updating its index...\r\n\r\n"
			"Scanned %d files, %d/%d layers\r\n"
			"Found %d airports", files1, layers1, totalLayers1, airports1);
	FSUpdateModelessMessage(handle, buf);
	if (updateLayers) FSUpdateModeless(handle, layers1);
}

void progressSetTotalLayersCountFS(int layersCount) {
	if (!setupOK) {
		progressSetTotalLayersCountBasic(layersCount);
		return;
	}
	totalLayers1 = layersCount;
	handle = FSCreateModeless(NULL, layersCount, NULL);
	updateWindow(TRUE);
	FSShowModeless(handle,TRUE);
}

void progressLayerDoneFS() {
	if (!setupOK) {
		progressLayerDoneBasic();
		return;
	}
	++layers1;
	updateWindow(TRUE);
}

void progressFinalizeFS() {
	if (!setupOK) {
		progressFinalizeBasic();
		return;
	}
	Sleep(config->progressDisplayDelay);
	FSShowModeless(handle, FALSE);
	FSCloseModeless(handle);
}

void progressIncrementFileCountFS(int inc) {
	if (!setupOK) {
		progressIncrementFileCountBasic(inc);
		return;
	}
	files1 += inc;
	if (inc != 0) updateWindow(FALSE);
}

void progressIncrementAirportCountFS(int inc) {
	if (!setupOK) {
		progressIncrementAirportCountBasic(inc);
		return;
	}
	airports1 += inc;
	if (inc != 0) updateWindow(FALSE);
}
