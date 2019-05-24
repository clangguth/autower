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


#include "icaolist.h"
#include "display.h"

AirportInfo** array = NULL;
unsigned int capacity = 0;
unsigned int count = 0;

unsigned int getIcaoListSize() {
	return count;
}
AirportInfo* getAirportInIcaoList(int position) {
	return array[position];
}

void freeIcaoList() {
	free(array);
}

int binarySearch(char *icao) {
	int position = 0;
	int begin = 0;
	int end = count - 1;
	int cmp = 0;

	while(begin <= end) {
		position = (begin + end) / 2;
		if((cmp = strcmp(icao, array[position]->icao)) == 0)
			return position;
		if(cmp > 0)
			begin = position + 1;
		else
			end = position - 1;
	}

	if (count != 0 && strcmp(icao, array[position]->icao) > 0) ++position;
	return -(position+1);
}

AirportInfo* findAirportInIcaoList(char* icao) {
	int position = binarySearch(icao);
	if (position >= 0) {
		return array[position];
	}
	return NULL;
}

void dumpArray() {
	int i;
	for (i = 0; i < count; ++i) {
		display(DISPLAY_FATAL, array[i]->icao);
	}
	ExitProcess(1);
}
void ensureCapacity() {
	if (capacity >= count+1) return;
	AirportInfo** newArray;
	unsigned int newCapacity;
	if (capacity == 0) {
		newCapacity = 32 * 1024;
	} else {
		newCapacity = capacity * 2;
	}
	newArray = malloc(newCapacity * 4);
	if (array != NULL) {
		memcpy(newArray, array, capacity*4);
		free(array);
	}
	array = newArray;
	capacity = newCapacity;
}

void insertAirportInIcaoList(AirportInfo* airport) {
	ensureCapacity();
	int where = binarySearch(airport->icao);
	if (where < 0) {
		where = - (where +1);
	}

	LPVOID from = &array[where];
	LPVOID to = &array[where+1];
	int mov = (count++ - where)*4;
	memmove(to, from, mov);
	array[where] = airport;
}
