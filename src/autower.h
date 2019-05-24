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

#ifndef AUTOWER_H_
#define AUTOWER_H_

#include <windows.h>
#include "md5Usage.h"
#include "product.h"

#define AIRPORT_NAME_SIZE 32
#define AIRPORT_COM_SIZE 8


#define DATAFILE_REL "autower.dat"
#define INIFILE_REL "autower.ini"

// between version 2 and 3, there have actually not been any structural changes,
// but the detection of what should be indexed has changed, so an update is required.
// version 4 introduces new field towerFlags into AirportInfo.
#define DATFILE_VERSION 4

#define ICAOCODE = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define RADIUS 3443.9
#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816
#define MAINLOOP_SLEEP_INTERVAL 10

#define TOWERPOS_SIMPLEFIX 1
#define TOWERPOS_LAT_TWR 2
#define TOWERPOS_LAT_AP 4
#define TOWERPOS_LAT_CFG 8
#define TOWERPOS_LON_TWR 16
#define TOWERPOS_LON_AP 32
#define TOWERPOS_LON_CFG 64

// since DATFILE_VERSION 4 (autower 2.3.0)
#define TOWERPOS_OVERRIDE_ALT 1
#define TOWERPOS_OVERRIDE_LATLON 2


#pragma pack(1)
typedef struct RunwayInfoBuildtime {
	BYTE surface;
	BYTE primaryNumber;
	BYTE secondaryNumber;
	BYTE designators;
	DWORD longitude;
	DWORD latitude;
	float length;
	float heading;
	struct RunwayInfoBuildtime* next;
} RunwayInfoBuildtime;

typedef union {
	float singleRunwayInfo;
	RunwayInfoBuildtime* runways;
} RunwayInfo;

#pragma pack(1)
typedef struct AirportInfo {
	char icao[7];
	unsigned char runways;
	RunwayInfo runwayInfo;
	char name[AIRPORT_NAME_SIZE];
	WORD com[AIRPORT_COM_SIZE];
	DWORD longitude;
	DWORD latitude;
	DWORD altitude;
	DWORD towerLongitude;
	DWORD towerLatitude;
	DWORD towerAltitude;
	float currentDistance;
	char towerFlags; // since DATFILE_VERSION = 4;
	char unused[3]; // since DATFILE_VERSION = 4;
} AirportInfo, *PAirportInfo;

/*struct AirportListEntry;

typedef struct AirportListEntry {
	AirportInfo* airport;
	struct AirportListEntry* next;
} AirportListEntry;
*/

typedef double ElementType;
typedef ElementType LatLon[2];

struct KdNode;
typedef struct KdNode *Position;
typedef struct KdNode *KdTree;

struct KdNode
{
	AirportInfo *payload;
	LatLon Data;
	KdTree   Left;
	KdTree   Right;
};


/* SHARED PROTOTYPES */

KdTree treeInsertAirport( LatLon Item, AirportInfo* payload, KdTree T );
void treeFindAirport( LatLon Low, LatLon High, KdTree T);
void treeFindResultCallback(AirportInfo* nextResult);
void makeAbsolutePath(char* filename);

#endif /* AUTOWER_H_ */
