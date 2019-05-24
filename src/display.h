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

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define WINDOW_TITLE "autower 2.0"

#define DISPLAY_DEBUG 5
#define DISPLAY_DETAIL 4
#define DISPLAY_INFO 3
#define DISPLAY_WARN 2
#define DISPLAY_ERROR 1
#define DISPLAY_FATAL 0

#include <windows.h>

void setDisplayLevel(int level);
void display(int level, const char *fmt, ...);
LPVOID getFormattedErrorMessage();

#endif /* DISPLAY_H_ */
