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

#include "memmap.h"
#include "display.h"
#include <stdio.h>

BOOL mapFile(char* filename, MappedFile* target) {
	target->hFile = INVALID_HANDLE_VALUE;
	target->hMap = INVALID_HANDLE_VALUE;
	target->base = NULL;

	target->hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (target->hFile == INVALID_HANDLE_VALUE) {
		LPTSTR err = getFormattedErrorMessage();
		display(DISPLAY_INFO, "Unable to open file \"%s\", error was: %s\n", filename, err);
		LocalFree(err);
		return 0;
	}

	target->hMap = CreateFileMappingA(target->hFile, NULL, PAGE_READONLY,0,0,NULL);
	if (target->hMap == INVALID_HANDLE_VALUE) {
		LPTSTR err = getFormattedErrorMessage();
		display(DISPLAY_ERROR, "Unable to create file mapping \"%s\", error was: %s\n", filename, err);
		LocalFree(err);
		return unmapFile(target);
	}

	target->base = MapViewOfFile(target->hMap,FILE_MAP_READ,0,0,0);
	if (target->base == NULL) {
		LPTSTR err = getFormattedErrorMessage();
		display(DISPLAY_ERROR, "Unable to map file \"%s\", error was: %s\n", filename, err);
		LocalFree(err);
		return unmapFile(target);
	}
	return TRUE;
}

BOOL unmapFile(MappedFile* map) {
	if (map->base != NULL) UnmapViewOfFile(map->base);
	if (map->hMap != INVALID_HANDLE_VALUE) CloseHandle(map->hMap);
	if (map->hFile != INVALID_HANDLE_VALUE) CloseHandle(map->hFile);
	return FALSE;
}
