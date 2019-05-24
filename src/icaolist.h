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


#ifndef ICAOLIST_H_
#define ICAOLIST_H_
#include "autower.h"

AirportInfo* findAirportInIcaoList(char* icao);
void insertAirportInIcaoList(AirportInfo* airport);
unsigned int getIcaoListSize();
AirportInfo* getAirportInIcaoList(int position);
void freeIcaoList();

#endif /* ICAOTREE_H_ */
