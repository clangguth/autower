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


#ifndef ICAOLIST_H_
#define ICAOLIST_H_
#include "autower.h"

AirportInfo* findAirportInIcaoList(char* icao);
void insertAirportInIcaoList(AirportInfo* airport);
unsigned int getIcaoListSize();
AirportInfo* getAirportInIcaoList(int position);
void freeIcaoList();

#endif /* ICAOTREE_H_ */
