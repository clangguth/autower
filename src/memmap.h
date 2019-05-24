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

#ifndef MEMMAP_H_
#define MEMMAP_H_

#include <windows.h>

typedef struct MappedFile {
	HANDLE hFile;
	HANDLE hMap;
	LPVOID base;
} MappedFile;

BOOL mapFile(char* filename, MappedFile* target);
BOOL unmapFile(MappedFile* map);

#endif /* MEMMAP_H_ */