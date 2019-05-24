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

#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "product.h"

#define DISPLAY_DEBUG 5
#define DISPLAY_DETAIL 4
#define DISPLAY_INFO 3
#define DISPLAY_WARN 2
#define DISPLAY_ERROR 1
#define DISPLAY_FATAL 0
#define DISPLAY_ALWAYS DISPLAY_FATAL

/* display level "up to" which messages are actually shown by default, unless log level is changed */
#define DISPLAY_DEFAULT DISPLAY_WARN

#include <windows.h>

void setDisplayLevel(int level);
void display(int level, const char *fmt, ...);
LPVOID getFormattedErrorMessage();

#endif /* DISPLAY_H_ */
