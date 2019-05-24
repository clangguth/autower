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

#ifndef CONFIG_H_
#define CONFIG_H_

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
#define DEFAULT_ZOOMLEVEL 384
#define DEFAULT_UPDATEINTERVAL 5000
#define DEFAULT_DISPLAYDURATION 5
#define DEFAULT_SIMPLEFIXHEADING 90
#define DEFAULT_SIMPLEFIXDISTANCE 10
#define DEFAULT_ALWAYSSETTOWER 0
#define DEFAULT_FORCEFSPATHTOUNC 0

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
} Configuration;

Configuration* configInitialize();
void configRead(Configuration* config, char* iniFile);

#endif /* CONFIG_H_ */
