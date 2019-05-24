/*
    This file is part of autower, Copyright (C) Christoph Langguth

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

#ifndef BGL_H_
#define BGL_H_

#define SCENERY_DAT_INDEX_BIT_AIRPORT 0x2000
#define SECTION_TYPE_AIRPORT 3
#define RECORD_TYPE_AIRPORT 3
#define SUBRECORD_TYPE_NAME 0x19
#define SUBRECORD_TYPE_COM 0x12
#define SUBRECORD_TYPE_RUNWAY 4
#define SUBRECORD_TYPE_DELETE 0x33
#define COM_TYPE_TOWER 6
#define DELETE_FLAG_ALL_FREQUENCIES (1<<3)
#define DELETE_FLAG_ALL_RUNWAYS (1<<5)

typedef struct SceneryDatHeader {
    DWORD magic;
    DWORD unknown[7];
    DWORD entryCount;
    DWORD indexedTypes;
    DWORD unknown2[17];
} SceneryDatHeader;

typedef struct SceneryDatEntry {
    DWORD size;
    DWORD unknown;
    DWORD indexedTypes;
    DWORD unknown2[6];
    DWORD qmidCount;
    DWORD qmids[8];
    DWORD unknown3[4];
    DWORD filenamelength;
    char filename[];
} SceneryDatEntry;

typedef struct BglFileHeader {
	DWORD magic;
	DWORD headerSize;
	DWORD unknown1[3];
	DWORD sections;
	DWORD unknown2[8];
} BglFileHeader;

typedef struct BglSectionPointer {
	DWORD type;
	DWORD unknown;
	DWORD subsections;
	DWORD offset;
	DWORD sectionSize;
} BglSectionPointer;

typedef struct BglSubsectionPointer {
	DWORD qmid;
	DWORD records;
	DWORD offset;
	DWORD size;
} BglSubsectionPointer;

#pragma pack(2)
typedef struct BglRecordHeader {
	WORD type;
	DWORD size;
} BglRecordHeader;

#pragma pack(1)
typedef struct BglAirportRecord {
	BglRecordHeader header;
	BYTE runwaySubrecords;
	BYTE comSubrecords;
	BYTE startSubrecords;
	BYTE approachSubrecords;
	BYTE apronsAndDelete;
	BYTE helipadSubrecords;
	DWORD airportLongitude;
	DWORD airportLatitude;
	DWORD airportAltitude;
	DWORD towerLongitude;
	DWORD towerLatitude;
	DWORD towerAltitude;
	float magneticVariation;
	DWORD icao;
	DWORD unknown[2];
} BglAirportRecord;

#pragma pack(1)
typedef struct BglRunwaySubrecord {
	BglRecordHeader header;
	WORD surface;

	BYTE primaryNumber;
	BYTE primaryDesignator;
	BYTE secondaryNumber;
	BYTE secondaryDesignator;
	BYTE ignore[8];
	DWORD longitude;
	DWORD latitude;
	DWORD ignore2;
	float length;
	float ignore3;
	float heading;
} BglRunwaySubrecord;

#pragma pack(1)
typedef struct BglComSubrecord {
	BglRecordHeader header;
	WORD type;
	DWORD frequency;
} BglComSubrecord;

#pragma pack(1)
typedef struct BglDeleteSubrecord {
	BglRecordHeader header;
	WORD deleteFlags;
	BYTE runwayCount;
	BYTE startCount;
	BYTE frequencyCount;
	BYTE unused;
} BglDeleteSubrecord;

#pragma pack(1)
typedef struct BglDeleteRunwaySubsubrecord {
	BYTE surface;
	BYTE primaryNumber;
	BYTE secondaryNumber;
	BYTE designators;
} BglDeleteRunwaySubsubrecord;

#pragma pack(1)
typedef struct BglDeleteFrequencySubsubrecord {
	DWORD typeAndFrequency;
} BglDeleteFrequencySubsubrecord;

#endif /* BGL_H_ */
