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

#include "md5Usage.h"
#include "md5.h"
#include "memmap.h"

BOOL md5Equals(MD5* first, MD5* second) {
	return first->high == second->high && first->low == second->low;
}

void md5Digest(LPVOID digest, LPVOID start, int length) {
	md5_state_t state;
	//md5_byte_t digest[16];

	md5_init(&state);
	md5_append(&state, start, length);
	md5_finish(&state, digest);
}

void md5ForMappedFile(MD5* result, MappedFile* map) {
	md5Digest(result, map->base, GetFileSize(map->hFile, NULL));
}

BOOL md5ForFile(MD5* result, char* filename) {
	MappedFile map;
	if (!mapFile(filename, &map)) return FALSE;
	md5ForMappedFile(result, &map);
	unmapFile(&map);
	return TRUE;
}

