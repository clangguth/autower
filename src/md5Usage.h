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
