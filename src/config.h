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

#ifndef CONFIG_H_
#define CONFIG_H_
#include <windows.h>

#define INI_SECTIONKEY "AutowerConfig"

#define DEFAULT_COMLOCKTHRESHOLD 30
#define DEFAULT_MAXDISPLAYINFOALTITUDE 3042
#define DEFAULT_TOWERALT1RW 30
#define DEFAULT_TOWERALT2RW 50
#define DEFAULT_TOWERALT3RW 100
#define DEFAULT_TOWERALT4RW 140
#define DEFAULT_TOWERALT5RW 140
#define DEFAULT_TOWERALT6RW 140
#define DEFAULT_TOWERMINCANDIDATES 100
#define DEFAULT_ZOOMLEVEL 256
#define DEFAULT_UPDATEINTERVAL 5000
#define DEFAULT_DISPLAYDURATION 5
#define DEFAULT_SIMPLEFIXHEADING 90
#define DEFAULT_SIMPLEFIXDISTANCE 10
#define DEFAULT_ALWAYSSETTOWER 0
#define DEFAULT_FORCEFSPATHTOUNC 0
#define DEFAULT_INTEGRATEDUI 1
#define DEFAULT_PROGRESSDISPLAYDELAY 2500

#define KEY_COMLOCKTHRESHOLD "ComLockThreshold"
#define KEY_MAXDISPLAYINFOALTITUDE "MaxDisplayInfoAltitude"
#define KEY_TOWERALT1RW "TowerHeight1Rw"
#define KEY_TOWERALT2RW "TowerHeight2Rw"
#define KEY_TOWERALT3RW "TowerHeight3Rw"
#define KEY_TOWERALT4RW "TowerHeight4Rw"
#define KEY_TOWERALT5RW "TowerHeight5Rw"
#define KEY_TOWERALT6RW "TowerHeight6Rw"
#define KEY_TOWERMINCANDIDATES "TowerMinCandidates"
#define KEY_ZOOMLEVEL "ZoomLevel"
#define KEY_UPDATEINTERVAL "UpdateInterval"
#define KEY_LOGLEVEL "LogLevel"
#define KEY_DISPLAYDURATION "DisplayDuration"
#define KEY_SIMPLEFIXHEADING "SimpleFixHeading"
#define KEY_SIMPLEFIXDISTANCE "SimpleFixDistance"
#define KEY_ALWAYSSETTOWER "AlwaysSetTower"
#define KEY_FORCEFSPATHTOUNC "ForceFsPathToUNC"
#define KEY_INTEGRATEDUI "IntegratedUI"
#define KEY_PROGRESSDISPLAYDELAY "ProgressDisplayDelay"

#define KEY_ICAO_NAME "Name"
#define KEY_ICAO_LATITUDE "Latitude"
#define KEY_ICAO_LONGITUDE "Longitude"
#define KEY_ICAO_ALTITUDE "Altitude"

/* user-definable variables */
typedef struct Configuration {
	unsigned int displayLevel;
	long maxDisplayInfoAltitude;
	unsigned int towerAlt1Rw;
	unsigned int towerAlt2Rw;
	unsigned int towerAlt3Rw;
	unsigned int towerAlt4Rw;
	unsigned int towerAlt5Rw;
	unsigned int towerAlt6Rw;
	unsigned int towerMinCandidates;
	unsigned int comLockThreshold;
	unsigned int zoomLevel;
	unsigned int updateInterval;
	unsigned int simpleFixHeading;
	int simpleFixDistance;
	int displayDuration;
	unsigned int alwaysSetTower;
	unsigned int forceFsPathToUNC;
	unsigned int integratedUI;
	unsigned int progressDisplayDelay;
	char* sections;
} Configuration;

Configuration* configInitialize();
void configRead(Configuration* config, char* iniFile);
void configFreeSections(Configuration* config);
BOOL configContainsICAOSection(Configuration* config, char* icao);

#endif /* CONFIG_H_ */
