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

#ifndef MD5USAGE_H_
#define MD5USAGE_H_

#include <windows.h>
#include "memmap.h"

typedef struct MD5 {
	unsigned long long low;
	unsigned long long high;
} MD5;

BOOL md5Equals(MD5* first, MD5* second);
BOOL md5ForFile(MD5* result, char* filename);
void md5ForMappedFile(MD5* result, MappedFile* map);

#endif /* MD5USAGE_H_ */
