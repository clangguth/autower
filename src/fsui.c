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


#include <stdio.h>
#include "fsui.h"
#include "display.h"
#include "linkage.h"
#include "config.h"

extern IMPORTTABLE ImportTable;
extern Configuration* config;

#define GET_ADDRESS 0x102


WINAPI LPVOID (*getAddress)(char* entry, char* module);
int __stdcall (*FSAlert)(char const * message, int buttons, int type);

int __stdcall FSAlertFallback(char const * message, int buttons, int type) {
	return MessageBoxA(NULL, message, PRODUCT_FULL, type);
}

void FSMessageBox(char* message, UINT type) {
	char buf[4096];
	char* display = message;
	getAddress = ImportTable.modules[IMPORT_FS9].functions[GET_ADDRESS];
	FSAlert = getAddress("?FSAlert@@YGHPBDHW4FSAlertType@@@Z", "fsui.dll");
	if (FSAlert != NULL && config->integratedUI) {
		/* simulate a "title", avoiding to waste vertical space by intending horizontally */
		strcpy(buf, "             -- ");
		strcat(buf, PRODUCT_FULL);
		strcat(buf, " --");
		switch (type) {
		case MB_ICONINFORMATION:
			type = 1;
			break;
		default:
			type = 0;
			break;
		}
		strcat(buf, "\r\n");
		strcat(buf, message);
		display = buf;
	} else {
		FSAlert = &FSAlertFallback;
	}
	FSAlert(display, 2, type);
}
