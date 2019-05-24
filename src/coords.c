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

#include <stdio.h>
#include "coords.h"
#include "windows.h"
#include "display.h"
#include "config.h"


double decodeAltitude(signed long dword) {
	return ((double)dword) / ((double)1000);
}

DWORD encodeAltitude(double d) {
	return (DWORD)(d * 1000.0);
}

double decodeLongitude(DWORD dword) {
	return ((double) dword) * (360.0 / (3* 0x10000000)) - 180.0;
}

DWORD encodeLongitude(double d) {
	return (DWORD)((d + 180.0) * ((3* 0x10000000) / 360.0));
}

double decodeLatitude(DWORD dword) {
	return 90.0 - ((double)dword) * (180.0 / (2* 0x10000000));
}

DWORD encodeLatitude(double d) {
	return (DWORD)((90.0 - d) * ((2* 0x10000000) / 180.0));
}


void overrideCoordinates(AirportInfo* airport, Configuration* config, char* iniFile) {
	char lat[32];
	char lon[32];
	char alt[32];

	GetPrivateProfileStringA(airport->icao, KEY_ICAO_LATITUDE, NULL, lat, 32, iniFile);
	GetPrivateProfileStringA(airport->icao, KEY_ICAO_LONGITUDE, NULL, lon, 32, iniFile);
	GetPrivateProfileStringA(airport->icao, KEY_ICAO_ALTITUDE, NULL, alt, 32, iniFile);

	if (alt[0] != 0) {
		double nalt;
		if (1 == sscanf(alt,"%lf",&nalt)) {
			display(DISPLAY_DEBUG, "Redefined tower altitude of %s to %f", airport->icao, nalt);
			airport->towerAltitude = encodeAltitude(nalt);
			airport->towerFlags |= TOWERPOS_OVERRIDE_ALT;
		} else {
			display(DISPLAY_WARN, "Unable to parse " KEY_ICAO_ALTITUDE " setting of " INIFILE_REL ", section %s", airport->icao);
		}
	}
#define ILLEGAL_LATLON (4242.42)
	double nlat = ILLEGAL_LATLON;
	double nlon = ILLEGAL_LATLON;

	// formats accepted for both latitude and longitude:
	// [NSEW]\d*[ ]?\f[']? (pseudo-regular expression)
	if (lat[0] != 0) {
		int d = 0; double m = ILLEGAL_LATLON; char ns = 0; char s = 0;
		if (strchr(lat, ' ')) {
			// do nothing, space char is illegal
		} else if (4 == sscanf(lat, "%c%d*%lf%c", &ns, &d, &m, &s) && (ns == 'N' || ns == 'S') && s == '\'') {
			m = (ns == 'N' ? d + (m / 60.0) : - (d + (m / 60.0))) ;
		} else if (1 == sscanf(lat, "%lf", &m)) {
			// do nothing, m is already set by sscanf
		} else {
			m = ILLEGAL_LATLON; // just in case
		}
		if (m >= -90.0 && m <= 90.0) {
			nlat = m;
		}
		if (nlat == ILLEGAL_LATLON) {
			display(DISPLAY_WARN, "Unable to parse " KEY_ICAO_LATITUDE " setting of " INIFILE_REL ", section %s", airport->icao);
		}
	}
	if (lon[0] != 0) {
		int d = 0; double m = 0; char ew = 0; char s = 0;
		if (strchr(lon, ' ')) {
			// do nothing, space char is illegal
		} else if (4 == sscanf(lon, "%c%d*%lf%c", &ew, &d, &m, &s) && (ew == 'E' || ew == 'W') && s == '\'') {
			m = (ew == 'E' ? d + (m / 60.0) : - (d + (m / 60.0))) ;
		} else if (1 == sscanf(lon, "%lf", &m)) {
			// do nothing, m is already set by sscanf
		} else {
			m = ILLEGAL_LATLON; // just in case
		}
		if (m >= -180.0 && m <= 180.0) {
			nlon = m;
		}
		if (nlon == ILLEGAL_LATLON) {
			display(DISPLAY_WARN, "Unable to parse " KEY_ICAO_LONGITUDE " setting of " INIFILE_REL ", section %s", airport->icao);
		}
	}

	// check that both longitude and latitude are provided if either of them is redefined.
	if (lat[0] != 0 || lon[0] != 0) {
		if (nlat == ILLEGAL_LATLON || nlon == ILLEGAL_LATLON) {
			display(DISPLAY_WARN, "Check your " INIFILE_REL ", section %s: both " KEY_ICAO_LATITUDE " and " KEY_ICAO_LONGITUDE " must be correctly defined.", airport->icao);
		} else {
			airport->towerLatitude = encodeLatitude(nlat);
			airport->towerLongitude = encodeLongitude(nlon);
			airport->towerFlags |= TOWERPOS_OVERRIDE_LATLON;
			display(DISPLAY_DEBUG, "Redefined tower lat/lon of %s to %f/%f", airport->icao, nlat, nlon);
		}
	}
}
