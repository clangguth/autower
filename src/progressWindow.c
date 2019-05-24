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

#include "progressWindow.h"
#ifndef BUILD_DLL
#include "progressWindowBasic.h"
void progressCreateWindow() {
	progressCreateWindowBasic();
}
void progressIncrementFileCount(int inc) {
	progressIncrementFileCountBasic(inc);
}
void progressIncrementAirportCount(int inc) {
	progressIncrementAirportCountBasic(inc);
}
void progressSetTotalLayersCount(int layersCount) {
	progressSetTotalLayersCountBasic(layersCount);
}
void progressLayerDone() {
	progressLayerDoneBasic();
}
void progressFinalize() {
	progressFinalizeBasic();
}
#else
#include "progressWindowFS.h"
void progressCreateWindow() {
	progressCreateWindowFS();
}
void progressIncrementFileCount(int inc) {
	progressIncrementFileCountFS(inc);
}
void progressIncrementAirportCount(int inc) {
	progressIncrementAirportCountFS(inc);
}
void progressSetTotalLayersCount(int layersCount) {
	progressSetTotalLayersCountFS(layersCount);
}
void progressLayerDone() {
	progressLayerDoneFS();
}
void progressFinalize() {
	progressFinalizeFS();
}
#endif
