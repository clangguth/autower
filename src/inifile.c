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

#include "inifile.h"
#include "display.h"
#include <windows.h>

char* iniFileEnumerateSections(char* iniFile) {
	// we don't know the required length beforehand, so the loop below
	// adjusts the available length exponentially.
	unsigned int length = 32;
	char* sections;
	while (TRUE) {
		sections = malloc(length);
		if (sections == NULL) {
			break;
		}
		DWORD used = GetPrivateProfileStringA(NULL, NULL, NULL, &(sections)[1], length-1, iniFile);
		if (used == 0) {
			// no .ini file at all
			free(sections);
			sections = NULL;
			break;
		}
		if (used < length - 3) {
			// available length was definitely enough,
			// now convert NULL separators to spaces
			unsigned int i;
			for (i=0; i <= used; ++i) {
				if (i == 0 || sections[i] == '\0') {
					sections[i]= ' ';
				}
			}
			break;
		}
		length *= 2;
	}
	if (sections == NULL) {
		display(DISPLAY_DEBUG, "No sections found in %s, file doesn't exist or is invalid.", iniFile);
	} else {
		display(DISPLAY_DEBUG,"Found these sections in ini file %s:%s", iniFile, sections);
	}
	return sections;
}
