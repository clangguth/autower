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




/* As this is the main file, version history is tracked here.

Version history
===============

1.01 - November 28, 2006
- fixed reading of the initialization file for the command-line version. I knew
  something would go wrong :-/

1.02 - November 29, 2006
- Corrected the connection method to FSUIPC for the DLL version to use FSUIPC_Open2

1.03 - December 3, 2006
- File parsing is now done using memory mapped files. This gives a speedup of about 10%.
- Added the graphical progress display when building datafile.
- Added possibility for the simple fixes for airports with a single runway.

1.04 - December 9, 2006
- Slightly more logging
- More readable error messages

2.0 (April 21, 2010)

* Major rewrite of almost everything. The code has undergone very heavy
  refactoring, resulting in dramatically improved cleanliness and readability.
  Hey, it's been 3.5 years of learning since the first version! ;-)

* Combining the airport information in roughly the same way as FS itself does;
  autower now "refines" the airport information starting from the lowest layer,
  instead of taking whichever AFCAD was topmost and ignoring all others. This
  should give results that are way more correct for heavily modded airports.

* Database building sped up by several orders of magnitude. The biggest gain
  comes from the (partial) understanding and using of the scenery.dat indexes,
  instead of brute-force scanning every available BGL file.

* Automatic detection of scenery changes, which triggers an automatic rebuild
  of the data file. Change detection is done through MD5 sums.

* several smaller fixes to address various minor issues:
  - new option to forcibly set the tower position on every iteration. The most
    frequent use will probably be to work around other add-ons that muck around
    with the tower position, like FSHotSFX.
  - avoid having to use UNC paths by translating them to local paths. This
    handles situations where the "Server" service is unavailable better (for
    example, AlacrityPC shuts down that service), generally seems faster, and is
    also less prone to weird "permission denied" errors when requesting write
    access via UNC (which, in contrast, is perfectly fine via a local path)
  - different handling of slow FS startups, now progressively gives FS more time
    to get initialized, and keeps asking whether it should continue.

2.01 - April 24, 2010
  - bugfix to handle UNC paths better. In fact, I'm now ignoring UNC as much as I can,
    because it proved to be a freaking mess. Instead, I get the path from the running
    FS9 process.

2.02 - April 25, 2010
  - Sporadically, the DLL variant would cause an FS crash on shutdown. The shutdown
    is now better coordinated so that this (hopefully) does not happen anymore.

2.03 - April 25, 2010
  - Lowered the priority of the message that an airport section has an unknown section
    code from "warning" to "detail". The message was reported to show up for the Aerosoft
    German Airports 3 X - Paderborn-Lippstadt EDLP Exclude BGL, but does not seem to have
    other side effects.

2.1.0 - May 15, 2010
  - changed FSUIPC detection code for the DLL
  - added UI integration for the DLL
  - made progress window delay customizable
  - changed scenery.cfg parsing to be case-insensitive for the "active" attribute
  - minimal performance gain for DLL: DisableThreadLibraryCalls()
  - changed default zoom level from 6 to 4
  - removed some obsolete parameters to functions (code cleanup)
  - adjusted documentation
  - changed version number scheme, and included metadata information in binary files

2.1.1 - May 21, 2010
  - bugfix for negative (airport/tower) altitudes: were treated as unsigned before,
    resulting in an extremely high tower position

2.1.2 - some time in 2010, not publicly released
  - for very special setups, a single airport may have multiple AFCADs (one real, the
    others dummy). These AFCADs share the same coordinates, so autower has a hard time
    finding out which one is real, and which one is dummy. Dummy BGLs typically don't
    make use of tower locations, so a simple possibility to flag an airport BGL as "dummy"
    is to set its altitude to an unrealistic/physically impossible value. Any BGL
    reporting a tower altitude of 1 km or more *below* sea level is thus ignored by autower
    (treated as if it didn't exist).

2.2.0 - Feb 26, 2011
  - minor bugfix for displaying messages (wrong icon)
  - default message display level changed to WARN (instead of INFO).
  - now also considers "remote" scenery locations, and honors "required" attribute.
  - dealing with missing sceneries refined:
    - display a DETAIL message if scenery is unaccessible and not required
    - display an INFO message if scenery is unaccessible but required.
    - these changes, together with the change to the default log/display level, should make
      startup more smooth in normal scenarios -- at the expense of users not knowing that there
      might be a problem in their configuration.
 	 - various refactorings.

2.3.0 - Oct 30, 2011:
	- ignoring runways which are closed both on primary and secondary headings (as FS itself does).
	- allow to override airport/tower name, lat/lon, and alt in the configuration by
	  creating the corresponding sections.
	- documentation/source comments cleanup and additions
	- license clarification: previous versions were ambiguous about which version applies;
	  from now on, it should be clear that GPLv2 only is in effect.

2.4.0 - Nov 04, 2011:
	- completely rewritten logic for interpreting the areas in scenery.cfg. Layers are
	  now ordered exactly as in FS (using the 'Layer' setting only, ignoring the section name),
	  and an arbitrary number of layers is supported.
	- fixed a small glitch that was introduced in 2.3.0 (forgot to remove some development-only
	  messages, which would pop up unconditionally)
	- adjusted (lowered) priority of some messages, to de-clutter the DETAIL log
	  level.
	- small fixes in autower.ini
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <psapi.h>
#include "autower.h"
#include "FSUIPC_User.h"
#include "progressWindow.h"
#include "bgl.h"
#include "display.h"
#include "memmap.h"
#include "md5Usage.h"
#include "config.h"
#include "icaolist.h"
#include "coords.h"
#include "scenerycfg.h"

/*  This stuff is required for building the DLL only.
	Pass the -DBUILD_DLL option to the compiler.
*/
#ifdef BUILD_DLL
#include "linkage.h"

/* These are the functions called when loading and unloading the module */
void FSAPI module_init ();
void FSAPI module_deinit ();

/* The global exported LINKAGE object */
LINKAGE Linkage =
{
    0x00000000,
    module_init,
    module_deinit,
    0,
    0,
    0x900,
    { 0 }
};

/* The global exported IMPORTTABLE object */
IMPORTTABLE ImportTable =
{
    {
            { IMPORT_FS9_ID, NULL },
            { IMPORT_FSUIPC_ID, NULL },
            { 0x00000000, NULL },
    }
}; 
BYTE dllIpcMem[1024];
#define FSUIPC_OPEN FSUIPC_Open2(SIM_ANY, &ipcResult, dllIpcMem, sizeof(dllIpcMem))
#else /* building the executable variant */
#define FSUIPC_OPEN FSUIPC_Open(SIM_ANY, &ipcResult)
#endif

/* GLOBAL VARIABLES */
Configuration* config = NULL;

/* FS and autower files */
char fsBaseDir[MAX_PATH] = {0};
char dataFile[MAX_PATH] = {0};
char iniFile[MAX_PATH] = {0};

/* own data file handle */
HANDLE hDataFile = NULL;

SceneryInfo* sceneryInfo = NULL;

/* Airports tree & search variables */
KdTree airportsTree = NULL;
int airportSearchResultCount = 0;
AirportInfo** airportSearchResults = NULL;

/* current game state */
double currentLatitude = 0;
double currentLongitude = 0;
double currentAltitude = 0;
WORD currentActiveCom = 0;
AirportInfo* currentAirport = NULL;

/* IPC result */
DWORD ipcResult = 0;
HANDLE ipcMutex = NULL;
BOOL stopRequested = FALSE;

/* convenience macro */
#define DEGREE_TO_RADIAN(VAR) (((float)((int)VAR))*PI)/180.0

const char *IPC_ERROR_MESSAGES[] = {
	"Okay",
	"Attempt to Open when already Open",
	"Cannot link to FSUIPC or WideClient",
	"Failed to Register common message with Windows",
	"Failed to create Atom for mapping filename",
	"Failed to create a file mapping object",
	"Failed to open a view to the file map",
	"Incorrect version of FSUIPC, or not FSUIPC",
	"Sim is not version requested",
	"Call cannot execute, link not Open",
	"Call cannot execute: no requests accumulated",
	"IPC timed out all retries",
	"IPC sendmessage failed all retries",
	"IPC request contains bad data",
	"Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
	"Read or Write request cannot be added, memory for Process is full",
};

int parseSceneryDat(LPSTR sceneryDat);
int buildDatabase();
int parseAirportsInBglFile(LPSTR bglFile);
int parseAirportsInBglFileMap(HANDLE hFile, LPSTR bglFile);
void dumpAirportInfo(AirportInfo *airport, int debugLevel);
double decodeAltitude(signed long dword);
double decodeLatitude(DWORD dword);
double decodeLongitude(DWORD dword);
void convertIcao(DWORD code, char* output);
float printableCom(DWORD encoded);
int calculateTowerPosition(double* lon, double* lat);
double calculateTowerAltitude();

DWORD WINAPI commonMain();
void makeAbsolutePath(char* filename);

void requestStop() {
	/* 10 seconds should be a reasonable timeout */
	DWORD shouldRelease = WaitForSingleObject(ipcMutex, 10000);
	shouldRelease = shouldRelease != WAIT_FAILED && shouldRelease != WAIT_TIMEOUT;
	stopRequested = TRUE;
	if (shouldRelease) ReleaseMutex(ipcMutex);
}

#ifdef BUILD_DLL
void FSAPI module_init () {
	if (ImportTable.modules[IMPORT_FSUIPC].functions == NULL) {
		display(DISPLAY_FATAL, "You do not seem to have the FSUIPC module installed. "
		PRODUCT " will not work."
		);
		return;
	}
	commonMain();
}

void FSAPI module_deinit () {
	requestStop();
}

BOOL WINAPI DllMain (HANDLE hDll, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		config = configInitialize();
		DisableThreadLibraryCalls(hDll);
	}
	return TRUE;
}

BOOL isDebuggedAirportSet() {
	return FALSE;
}

BOOL isDebuggedAirport(AirportInfo* airport) {
	return FALSE;
}

void debugAirport(char* bglFile, AirportInfo* airport) {
}

#else /* exe */

BOOL WINAPI consoleHandler(DWORD event) {
	requestStop();
	Sleep(MAINLOOP_SLEEP_INTERVAL * 5);
	return TRUE;
}

char debuggedAirport[8] = {0};
int main(int argc, char** argv) {
	SetConsoleTitleA(PRODUCT_FULL);
	config = configInitialize();
	if (argc >= 2) {
		strncpy(debuggedAirport, argv[1], 7);
		display(DISPLAY_FATAL, "Debugging airport: %s", debuggedAirport);
	}
	return commonMain(NULL);
}


BOOL isDebuggedAirportSet() {
	return debuggedAirport[0] != 0;
}

BOOL isDebuggedAirport(AirportInfo* airport) {
	return isDebuggedAirportSet() && (!strcmpi(debuggedAirport, airport->icao));
}

void decodeAndDisplayTowerPositionFlags(unsigned int debuglevel, int towerPositionFlags) {
	char buf[512];
	strcpy(buf, "\tTower coordinates: lat from ");
	if (towerPositionFlags & TOWERPOS_LAT_AP) {
		strcat(buf, "airport");
	} else if (towerPositionFlags & TOWERPOS_LAT_TWR){
		strcat(buf, "tower");
	} else {
		strcat(buf, "config");
	}
	strcat(buf, ", lon from ");
	if (towerPositionFlags & TOWERPOS_LON_AP) {
		strcat(buf, "airport");
	} else if (towerPositionFlags & TOWERPOS_LON_TWR){
		strcat(buf, "tower");
	} else {
		strcat(buf, "config");
	}
	if (towerPositionFlags & TOWERPOS_SIMPLEFIX) {
		strcat(buf, " + simple fix");
	}
	display(debuglevel, buf);
}

void debugAirport(char* bglFile, AirportInfo* airport) {
	if (!isDebuggedAirport(airport)) return;
	BOOL final = bglFile == NULL;
	if (!final)
		display(DISPLAY_ALWAYS, "\r\n%s after processing %s", airport->icao, bglFile);
	else
		display(DISPLAY_ALWAYS, "\r\nFINAL RESULT FOR %s (%s)", airport->icao, airport->name);
	unsigned int count = 0;
	while (airport->com[count] != 0) {
		display(DISPLAY_ALWAYS, "\tTower frequency: %.3f", printableCom(airport->com[count++]));
	}
	if (!final) {
		count = 0;
		RunwayInfoBuildtime* runway = airport->runwayInfo.runways;
		while (runway != NULL) {
			++count;
			runway = runway->next;
		}
	} else {
		count = airport->runways;
	}
	display(DISPLAY_ALWAYS, "\tNumber of runways: %d", count);

	double alt = decodeAltitude(airport->altitude);
	double lon = decodeLongitude(airport->longitude);
	double lat = decodeLatitude(airport->latitude);
	display(DISPLAY_ALWAYS, "\tAirport location (lat,lon,alt): (%f, %f, %f)", lat, lon, alt);

	if (!final) {
		lon = decodeLongitude(airport->towerLongitude);
		lat = decodeLatitude(airport->towerLatitude);
		alt = decodeAltitude(airport->towerAltitude);
		display(DISPLAY_ALWAYS, "\t  Tower location (lat,lon,alt): (%f, %f, %f)", lat, lon, alt);
	} else {
		currentAirport = airport;
		int towerPositionFlags = calculateTowerPosition(&lon, &lat);
		alt = calculateTowerAltitude();
		display(DISPLAY_ALWAYS, "\t  Tower location (lat,lon,alt): (%f, %f, %f)", lat, lon, alt);
		char buf[256];
		if ((airport->towerFlags & TOWERPOS_OVERRIDE_ALT) == TOWERPOS_OVERRIDE_ALT) {
			strcpy(buf, "overridden in configuration");
		}
		else if (decodeAltitude(airport->towerAltitude) == 0) {
			int runways = airport->runways;
			if (runways < 1) runways = 1;
			else if (runways > 6) runways = 6;
			sprintf(buf, "calculated using TowerHeight%dRw", runways);
		} else {
			strcpy(buf, "taken from AFCAD");
		}
		display(DISPLAY_ALWAYS, "\tTower altitude: %s",buf);
		decodeAndDisplayTowerPositionFlags(DISPLAY_ALWAYS, towerPositionFlags);
	}
}
#endif

BOOL connectToFSUIPC() {

#ifdef BUILD_DLL
	BOOL shutup = TRUE;
#else
	BOOL shutup = FALSE;
#endif

	BOOL success = FSUIPC_OPEN || ipcResult < 2;
	if (!success && !shutup) {
		display(DISPLAY_FATAL, "FSUIPC connection failed: %s",IPC_ERROR_MESSAGES[ipcResult]);
	}
	return success;
}

float printableCom(DWORD encoded) {
	float result = ((float) encoded + 117000) / 1000;
	return result != 117.0 ? result : 0;
}

double getDistance(double lat1, double lon1, double lat2, double lon2) {
	/* distance in nautical miles */
	double rlat1, rlat2, rlon1, rlon2;
	rlat1 = lat1*PI / 180.0;
	rlat2 = lat2*PI / 180.0;
	rlon1 = lon1*PI / 180.0;
	rlon2 = lon2*PI / 180.0;
	return acos(sin(rlat1)*sin(rlat2) +cos(rlat1)*cos(rlat2)*cos(rlon2-rlon1)) * RADIUS;
}

void treeFindResultCallback(AirportInfo* nextResult) {
	airportSearchResults[airportSearchResultCount++] = nextResult;
	nextResult->currentDistance = getDistance(
		currentLatitude,
		currentLongitude,
		decodeLatitude(nextResult->latitude),
		decodeLongitude(nextResult->longitude));
	//dumpAirportInfo(nextResult);
}

void executeSearchForBoundingBox(double minLat, double minLon, double maxLat, double maxLon, long factor) {
	LatLon min;
	LatLon max;
	min[0] = minLat;
	min[1] = minLon;
	max[0] = maxLat;
	max[1] = maxLon;

	long time = GetTickCount();
	treeFindAirport(min,max,airportsTree);
	time = GetTickCount() - time;
	display(DISPLAY_DEBUG,"search for (%f,%f)-(%f,%f) (factor %ld) took %ld ms, yielded %d results", minLat,minLon,maxLat,maxLon,factor,time, airportSearchResultCount);
}

BOOL doesAirportOfferCurrentRadioFreq(AirportInfo* airport) {
	int c;
	for (c = 0; c < AIRPORT_COM_SIZE; c++) {
		WORD freq = airport->com[c];
		if (freq == 0) return FALSE;
		/* The last digit is not used in FS, so remove it */
		freq -= freq % 10;

		if (freq == currentActiveCom) {
			if (currentAirport != airport) {
				display(DISPLAY_DEBUG, "COM Frequency matched for %s at distance %f", airport->icao, airport->currentDistance);
			}
			return TRUE;
		}
	}
	return FALSE;
}

AirportInfo* chooseBestInSearchResults() {
	int i;
	AirportInfo *closest = NULL;
	BOOL closestHasRadio = FALSE;
	for(i=airportSearchResultCount-1; i >= 0; --i) {
		AirportInfo *candidate = airportSearchResults[i];
		BOOL candidateHasRadio = FALSE;

		/* check the COM frequencies of this airport,
		but only if it's within COM_LOCK_THRESHOLD nm */
		if (candidate->currentDistance < config->comLockThreshold) {
			candidateHasRadio = doesAirportOfferCurrentRadioFreq(candidate);
		}
		if (closest == NULL || (candidateHasRadio && !closestHasRadio)) {
			/* this airport is better than the previous, either because there was no previous ;-)
			or because its COM frequency matches while the previous doesn't */
			closest = candidate;
			closestHasRadio = candidateHasRadio;
			continue;
		}

		if (!candidateHasRadio && closestHasRadio) {
			/* this airport is worse regardless of its distance */
			continue;
		}
		
		/* airports are equal wrt to radio availability - let the distance decide. */
		if (candidate->currentDistance < closest->currentDistance) {
			closestHasRadio = candidateHasRadio;
			closest = candidate;
		}
	}
	return closest;
}

AirportInfo* findCurrentlyClosestAirport(long factor, int minCount) {
	airportSearchResultCount = 0;
	if (factor < 2) factor = 2;
	double dFactor = factor;
	double minLat, minLon, maxLat, maxLon;

	dFactor /= 10;
	minLat = currentLatitude - dFactor / 2;
	minLon = currentLongitude - dFactor;

	maxLat = currentLatitude + dFactor / 2;
	maxLon = currentLongitude + dFactor;

	executeSearchForBoundingBox(minLat, minLon, maxLat, maxLon, factor);

	/* if we did not find enough airports, recurse */
	if (airportSearchResultCount < minCount && (minLat > -90 || maxLat < 90 || minLon > -360 || maxLon < 360)) {
		/* These strange values for longitude are due to the artificial entries */
		if (factor * 2 > factor) {
			/* recurse only if we won't get stuck, i.e.,
			prevent endless loops caused by overflows */
			return findCurrentlyClosestAirport(factor * 2, minCount);
		}
	}
	return chooseBestInSearchResults();
}

AirportInfo* findOverriddenAirport(AirportInfo* airport) {
	int debugLevel = DISPLAY_DETAIL;

	AirportInfo *existing =	findAirportInIcaoList(airport->icao);
	if (existing != NULL) {
		display(debugLevel, "Found an existing entry for %s", airport->icao);
		return existing;
	}
	return NULL;
}

void mergeTowerInfos(AirportInfo* lower, AirportInfo* higher) {
	BOOL overridden = FALSE;
	int debugLevel = DISPLAY_DEBUG;
	display(debugLevel, "Merging the tower info of the following two airports:");
	display(debugLevel, "Existing (lower layer):");
	dumpAirportInfo(lower, debugLevel);
	display(debugLevel, "New (higher layer):");
	dumpAirportInfo(higher, debugLevel);
	if (higher->towerLatitude != 0 && decodeLatitude(higher->towerLatitude) != 0) {
		overridden = TRUE;
		display(debugLevel, "merge overrides tower position");
		lower->towerAltitude = higher->towerAltitude;
		lower->towerLatitude = higher->towerLatitude;
		lower->towerLongitude = higher->towerLongitude;
	}

	if (overridden) {
		display(debugLevel, "Merged information:");
		dumpAirportInfo(lower, debugLevel);
	}
}

void fixupAirport(AirportInfo* airport) {
	if (configContainsICAOSection(config, airport->icao)) {
		display(DISPLAY_DETAIL, "Found custom configuration for %s", airport->icao);
		char name[AIRPORT_NAME_SIZE];
		GetPrivateProfileStringA(airport->icao, KEY_ICAO_NAME, NULL, name, AIRPORT_NAME_SIZE, iniFile);
		if (name[0]) {
			display(DISPLAY_DEBUG, "Changed name for %s: %s -> %s", airport->icao, airport->name, name);
			strcpy(airport->name, name);
		}
		overrideCoordinates(airport, config, iniFile);
	}
}

BOOL insertAirportInKdTree(AirportInfo *airport, int debugLevel) {

	LatLon location;
	location[0] = decodeLatitude(airport->latitude);
	location[1] = decodeLongitude(airport->longitude);

	dumpAirportInfo(airport, debugLevel);
	airportsTree = treeInsertAirport(location, airport, airportsTree);
	/* insert artificial nodes for coping with searches
	 * that are close to where longitude wraps from 180 to -180
	 */
	location[1] += (location[1] > 0 ? -1 : 1) * 360;
	airportsTree = treeInsertAirport(location, airport, airportsTree);
	return TRUE;
}

unsigned int readDatabaseHeader(unsigned int header) {
	if (header == 0) return 0;

	int version = (header >> 24);
	// to simulate response to another version, do something like this:
	//version = 1;

	/* the highest 8 bits of the count encode the data file version */
	if (version < DATFILE_VERSION) {
		display(DISPLAY_WARN, "%s%s%s%s",
			"A datafile from a previous version of " PRODUCT " has been detected. ",
			"Because of the update, the datafile needs to be rebuilt. ",
			"If you want to take advantage of possible new options, please make ",
			"sure you're using an up-to-date configuration file."
		);
		return 0;
	}
	if (version > DATFILE_VERSION) {
		display(DISPLAY_WARN, "%s%s%s",
			"A datafile from a NEWER version of " PRODUCT " has been detected.\r\n",
			"Because " PRODUCT " will not be able to work with this file, the datafile ",
			"has to be rebuilt."
		);
		return 0;
	}
	return header & 0xFFFFFF;
}

BOOL readDatabaseMd5(MD5* target) {
	DWORD nRead = 0;
	if (!ReadFile(hDataFile,target,sizeof(struct MD5),&nRead,NULL)) return FALSE;
	return (nRead == sizeof(struct MD5));
}

BOOL readDatabaseSceneryMd5() {
	MD5 md5;
	int i;
	if (!readDatabaseMd5(&md5)) return FALSE;
	if (!md5Equals(&md5, &(sceneryInfo->sceneryCfgMd5))) {
		display(DISPLAY_DETAIL, "scenery.cfg has changed, requesting database rebuild");
		return FALSE;
	}
	display(DISPLAY_DEBUG, "scenery.cfg has not changed");

	for (i=0; i < sceneryInfo->layersCount; ++i) {
		if (!readDatabaseMd5(&md5)) return FALSE;
		if (!md5Equals(&md5, sceneryInfo->layerMd5[i])) {
			display(DISPLAY_DETAIL, "Layer at index %d has changed, requesting database rebuild", i);
			return FALSE;
		}
		display(DISPLAY_DEBUG, "layer %d has not changed",i);
	}
	return TRUE;
}

int readDatabase() {
	DWORD nRead = 0;
	unsigned int count = 0;
	
	int i;
	AirportInfo *airports;

	ReadFile(hDataFile,&count,sizeof(DWORD),&nRead,NULL);
	if (nRead != sizeof(DWORD)) goto end;

	count = readDatabaseHeader(count);
	if (count == 0) goto end;
	
	if (!readDatabaseSceneryMd5()) {
		count = 0;
		goto end;
	}
	airports = malloc(count * sizeof(AirportInfo));
	ReadFile(hDataFile, airports, count * sizeof(AirportInfo),&nRead,NULL);
	
	for (i = 0; i < count; i++) {
		insertAirportInKdTree(&(airports[i]), DISPLAY_DEBUG);
		fixupAirport(&(airports[i]));
	}
end:
	CloseHandle(hDataFile);
	return count;
}

BOOL ipcReadIsFsReady() {
	BYTE readyToFly = 0;
	BYTE pausedInMenu = 0;
	WORD pausedInGame = 0;

	BOOL goOn = TRUE, success = TRUE;
	success &= FSUIPC_Read(0x3364, 1, &readyToFly, &ipcResult);
	success &= FSUIPC_Read(0x3365, 1, &pausedInMenu, &ipcResult);
	success &= FSUIPC_Read(0x0264, 2, &pausedInGame, &ipcResult);
	success &= FSUIPC_Process(&ipcResult);

	if (!success) {
		FSUIPC_Close();
		return FALSE;
	}

	readyToFly = !readyToFly; /* strange semantics here */
	goOn = success && readyToFly && !pausedInMenu && !pausedInGame;
	display(DISPLAY_DEBUG,"success: %d ready: %d pmenu: %d pgame: %d: goOn %d",success, readyToFly, pausedInMenu, pausedInGame, goOn);
	return goOn;
}

BOOL ipcReadRadioCom() {
	BOOL success = TRUE;
	BYTE radio = 0;
	WORD frequency = 0;

	success &= FSUIPC_Read(0x3122, 1, &radio, &ipcResult);
	success &= FSUIPC_Process(&ipcResult);
	if (!success) {
		FSUIPC_Close();
		return FALSE;
	}

	/* convert from FSUIPC bitmask to radio number */
	if (radio & 128) {
		radio = 1;
	} else if (radio & 64) {
		radio = 2;
	} else {
		radio = 0;
	}

	if (radio == 1) {
		success = FSUIPC_Read(0x034E, 2, &frequency, &ipcResult);
	} else if (radio == 2) {
		success = FSUIPC_Read(0x3118, 2, &frequency, &ipcResult);
	}
	success &= FSUIPC_Process(&ipcResult);
	if (!success) {
		FSUIPC_Close();
		return FALSE;
	}

	/* convert frequency representation and assign it to currentActiveCom */
	frequency = (frequency & 0xF) + ((frequency >> 4) & 0xF) * 10 + ((frequency >> 8) & 0xF) * 100 + ((frequency >> 12) & 0xF) * 1000;
	frequency = (((DWORD)(frequency + 10000)) * 10) - 117000;
	currentActiveCom = frequency;

	return TRUE;
}

BOOL ipcReadCurrentPosition() {
	long long ipcLatitude = 0;
	long long ipcLongitude = 0;
	long long ipcAltitude = 0;
	BOOL success = TRUE;

	success &= FSUIPC_Read(0x0560, 8, &ipcLatitude, &ipcResult);
	success &= FSUIPC_Read(0x0568, 8, &ipcLongitude, &ipcResult);
	success &= FSUIPC_Read(0x0570, 8, &ipcAltitude, &ipcResult);
	success &= FSUIPC_Process(&ipcResult);

	if (!success) {
		FSUIPC_Close();
		return FALSE;
	}

	currentLatitude = ((double)ipcLatitude) * 90.0/(10001750.0*65536*65536);
	currentLongitude = ((double)ipcLongitude) *360.0/(65536.0*65536*65536*65536);
	currentAltitude = ((double)ipcAltitude)/(65536.0*65536);

	return TRUE;
}

BOOL ipcReadData() {
	if (connectToFSUIPC()) {
		if (!ipcReadIsFsReady()) return FALSE;
		if (!ipcReadCurrentPosition()) return FALSE;
		if (!ipcReadRadioCom()) return FALSE;
		display(DISPLAY_DEBUG,"clat %f clon %f calt %f radio @%.3f", currentLatitude, currentLongitude, currentAltitude, (currentActiveCom+117000)/1000.0);
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ipcDisplay(char *text, int duration) {
	BOOL success = TRUE;
	char safeText[128] = {0};
	strncpy(safeText,text,127);
	
	if (!connectToFSUIPC()) {
		return FALSE;
	}
	success &= FSUIPC_Write(0x3380, sizeof(safeText), safeText, &ipcResult);
	success &= FSUIPC_Write(0x32FA, sizeof(duration), &duration, &ipcResult);
	success &= FSUIPC_Process(&ipcResult);
	if (!success) FSUIPC_Close();
	return success;
}

void applySimplePositionFix(double* lon, double *lat) {

	double rwyLength = ((int)currentAirport->runwayInfo.singleRunwayInfo)/10;
	double rwyHeading = (currentAirport->runwayInfo.singleRunwayInfo - rwyLength*10);
	double twrHeading = rwyHeading + DEGREE_TO_RADIAN(config->simpleFixHeading);

	if (twrHeading >= 2 *PI) twrHeading -= 2*PI;

	double distance = -config->simpleFixDistance;
	if (config->simpleFixDistance >= 0) {
		distance = rwyLength * config->simpleFixDistance / 100.0;
	}

	double metersLat = distance * cos(twrHeading);
	double metersLon = distance * sin(twrHeading);

	double nmLon = metersLon / 1852.01;
	double nmLat = metersLat / 1852.01;

	display(DISPLAY_DEBUG,"rwyLength: %f, rwyHdg: %f, twrHdg: %f, distance: %f", rwyLength, rwyHeading, twrHeading, distance);
	display(DISPLAY_DEBUG,"metersLat: %f, metersLon: %f => nmLat %f, nmLon %f", metersLat, metersLon, nmLat, nmLon);

	// adjust the offsets according to the latitude
	// 1 degree = 60 nm at the equator, adjusted by cos(lat) when going towards the poles
	nmLon = nmLon * cos(DEGREE_TO_RADIAN(*lat));
	nmLat = nmLat * cos(DEGREE_TO_RADIAN(*lat));

	display(DISPLAY_DEBUG,"lat %f lon %f, adjusted nmLat %f, nmLon %f", *lat, *lon, nmLat, nmLon);
	*lat += nmLat / 60.0;
	*lon += nmLon / 60.0;

	// just in case
	if (*lon < -180.0) *lon += 360.0;
	else if (*lon > 180.0) *lon -= 360.0;
}

double calculateTowerAltitude() {
	double alt = decodeAltitude(currentAirport->towerAltitude);
	if (((currentAirport->towerFlags & TOWERPOS_OVERRIDE_ALT) != TOWERPOS_OVERRIDE_ALT) && alt == 0.0) {
		switch (currentAirport->runways) {
		case 0:
		case 1:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt1Rw; break;
		case 2:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt2Rw; break;
		case 3:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt3Rw; break;
		case 4:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt4Rw; break;
		case 5:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt5Rw; break;
		default:
			alt = decodeAltitude(currentAirport->altitude) + config->towerAlt6Rw; break;
		}
	}
	return alt;
}


BOOL ipcSetTowerTo(double lat, double lon, double alt, BOOL setZoom) {
        long long ipcLat = (long long)(double)(lat * (10001750.0*65536*65536) / 90.0);
        long long ipcLon = (long long)(double)(lon * (65536.0*65536*65536*65536) / 360.0);
        long long ipcAlt = (long long)(double)(alt * (65536.0*65536));

        BOOL success = FSUIPC_Write(0x0D60, 8, &ipcAlt, &ipcResult);
        success &= FSUIPC_Write(0x0D50, 8, &ipcLat, &ipcResult);
        success &= FSUIPC_Write(0x0D58, 8, &ipcLon, &ipcResult);
        if (setZoom) {
                success &= FSUIPC_Write(0x8330, 2, &(config->zoomLevel), &ipcResult);
        }
        success &= FSUIPC_Process(&ipcResult);
        if (!success) FSUIPC_Close();
        return success;
}


int calculateTowerPosition(double* lon, double* lat) {
	int flags = 0;

	*lat = decodeLatitude(currentAirport->towerLatitude);
	if ((currentAirport->towerFlags & TOWERPOS_OVERRIDE_LATLON) != 0) {
		flags |= TOWERPOS_LAT_CFG;
	}
	else if (*lat == 0.0) {
		*lat = decodeLatitude(currentAirport->latitude);
		flags |= TOWERPOS_LAT_AP;
	} else {
		flags |= TOWERPOS_LAT_TWR;
	}

	*lon = decodeLongitude(currentAirport->towerLongitude);
	if ((currentAirport->towerFlags & TOWERPOS_OVERRIDE_LATLON) != 0) {
		flags |= TOWERPOS_LON_CFG;
	}
	else if (*lon == 0.0) {
		*lon = decodeLongitude(currentAirport->longitude);
		flags |= TOWERPOS_LON_AP;
	} else {
		flags |= TOWERPOS_LON_TWR;
	}

	if ((currentAirport->towerFlags & TOWERPOS_OVERRIDE_LATLON) == 0) {
		if (currentAirport->runways == 1 && currentAirport->runwayInfo.singleRunwayInfo != 0) {
			applySimplePositionFix(lon, lat);
			flags |= TOWERPOS_SIMPLEFIX;
		}
	}
	return flags;
}

void airportChanged(BOOL reallyChanged) {
	char buf[64];
	double lon, lat, alt;

	calculateTowerPosition(&lon, &lat);
	alt = calculateTowerAltitude();

	if (reallyChanged) {
		display(DISPLAY_DETAIL, "Setting tower to lat %f lon %f alt %f", lat, lon, alt);
		dumpAirportInfo(currentAirport, DISPLAY_DETAIL);
	}

	BOOL ipcWritten = ipcSetTowerTo(lat, lon, alt, reallyChanged);
	if (ipcWritten && reallyChanged && (currentAltitude <= (double)config->maxDisplayInfoAltitude)) {
		display(DISPLAY_DEBUG, "Displaying info because current alt %f <= %ld", currentAltitude, config->maxDisplayInfoAltitude);
		sprintf(buf, "Tower @ %s (%s)", currentAirport->icao, currentAirport->name);
		ipcDisplay(buf, config->displayDuration);
	}
	else {
		display(DISPLAY_DEBUG, "Not displaying info: ipcWritten (%d) != 1, or currentAltitude(%f) > maxDisplayInfoAltitude(%ld), or reallyChanged (%d) == 0", ipcWritten, currentAltitude, config->maxDisplayInfoAltitude, reallyChanged);
	}
}

/* This method signature is used because it's instantiated via CreateThread() for the DLL. */
DWORD WINAPI mainLoop(LPVOID unused) {
	unsigned int progress = config->updateInterval;

	// reset to NULL because debug functions at build time use this as well
	currentAirport = NULL;

	for (; !stopRequested; Sleep(MAINLOOP_SLEEP_INTERVAL)) {
		progress += MAINLOOP_SLEEP_INTERVAL;
		if (progress >= config->updateInterval) {
			progress = 0;
			WaitForSingleObject(ipcMutex, INFINITE);
			if (!stopRequested && ipcReadData()) {
				AirportInfo *newAirport = findCurrentlyClosestAirport(2, config->towerMinCandidates);
				BOOL reallyChanged = newAirport != currentAirport;
				currentAirport = newAirport;
				if (reallyChanged || config->alwaysSetTower) {
					airportChanged(reallyChanged);
				}
			}
			ReleaseMutex(ipcMutex);
		}
	}
#ifndef BUILD_DLL
	/* sometimes it's a bad idea to clean up after yourself.
	   This sometimes crashes FS for reasons unknown to me.
    */
	FSUIPC_Close();
#endif
	display(DISPLAY_DETAIL, "Main loop finished");
	return 0;
}

BOOL isAccessibleBaseDir(char* directory) {
	HANDLE fs9dir = CreateFile(directory, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (fs9dir != INVALID_HANDLE_VALUE) {
		CloseHandle(fs9dir);
		return TRUE;
	}
	return FALSE;
}


BOOL isUNCFileName(char* filename) {
	/* The shortest possible UNC would look like this:
	 * \\H\D\
	 */
	return strlen(filename) > 6 && filename[0] == '\\' && filename[1] == '\\';
}

BOOL getBasePathFromFS9(char* target) {
	HWND hwnd;
	DWORD pid = 0;
	HANDLE hProcess = NULL;
	HMODULE modules[1] = {0};
	DWORD modulesCount = 0;
	BOOL result = FALSE;

	hwnd = FindWindowEx(NULL, NULL, "FS98MAIN", NULL);
	if (!hwnd) return FALSE;

	GetWindowThreadProcessId(hwnd, &pid);
	if (!pid) return FALSE;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |PROCESS_VM_READ, FALSE, pid);
	if (!hProcess) return FALSE;

	if (!EnumProcessModules(hProcess, modules, 1, &modulesCount)) goto cleanup;
	if (!modulesCount) goto cleanup;

	GetModuleFileNameExA(hProcess, modules[0], target, MAX_PATH);
	if (isUNCFileName(target)) goto cleanup;
	if (strlen(target) < 3 || target[1] != ':') goto cleanup;

	int backslash = strlen(target) - 1;
	while (target[backslash] != '\\') {
		--backslash;
	}
	target[backslash+1] = '\0';
	result = TRUE;

	cleanup:
	CloseHandle(hProcess);
	return result;

}

BOOL findAlternativeBaseDir(char* alternative) {
	/* we only even try to find an alternative if the original is a UNC path */
	if (!isUNCFileName(fsBaseDir)) {
		display(DISPLAY_DEBUG, "Basedir is not a UNC path");
		return FALSE;
	}

	return getBasePathFromFS9(alternative);
}

BOOL setupChooseBaseDir() {
	char alternativeBaseDir[MAX_PATH] = {0};
	char buf[MAX_PATH*4];
	char* primary = NULL;
	char* secondary = NULL;

    primary = fsBaseDir;
    if (!config->forceFsPathToUNC && findAlternativeBaseDir(alternativeBaseDir)) {
		primary = alternativeBaseDir;
		secondary = fsBaseDir;
    }

    if (isAccessibleBaseDir(primary)) {
		if (primary != fsBaseDir) strcpy(fsBaseDir, primary);
		return TRUE;
    } else {
    	strcpy(buf, PRODUCT " was looking for the FS9 installation in ");
		strcat(buf, primary);
		strcat(buf,"\r\n");

		if (secondary != NULL) {
			if (isAccessibleBaseDir(secondary)) {
				if (secondary != fsBaseDir) strcpy(fsBaseDir, secondary);
				return TRUE;
			} else {
				strcat(buf, "It also tried ");
				strcat(buf, secondary);
				strcat(buf, "\r\nNone of these directories can be accessed, so " PRODUCT " will not function.");
			}
		} else {
			strcat (buf, "This directory cannot be accessed, so " PRODUCT " will not function.");
		}
		display(DISPLAY_FATAL, buf);
		return FALSE;
    }
    return TRUE;
}

BOOL setupBaseDirVariable() {
    BOOL success = FSUIPC_Read(0x3E00,MAX_PATH,fsBaseDir,&ipcResult);
    success &= FSUIPC_Process(&ipcResult);
    if (!success || strlen(fsBaseDir) < 1) {
		display(DISPLAY_FATAL, "Problem determining FS9 installation directory!");
		return FALSE;
	}
    display(DISPLAY_DEBUG, "FSUIPC returns basedir: %s", fsBaseDir);
	/* append trailing slash if needed */
    if(fsBaseDir[strlen(fsBaseDir) - 1] != '\\'){
        strcat(fsBaseDir, "\\");
    }
    if (!setupChooseBaseDir()) return FALSE;
    display(DISPLAY_DETAIL, "Using FS base directory: %s", fsBaseDir);
    return TRUE;
}

void setupOtherPathVariables() {
#ifdef BUILD_DLL
    sprintf(dataFile,"%s%s%s",fsBaseDir,"modules\\",DATAFILE_REL);
    sprintf(iniFile,"%s%s%s",fsBaseDir,"modules\\",INIFILE_REL);
#else
    sprintf(dataFile,".\\%s",DATAFILE_REL);
    sprintf(iniFile, ".\\%s",INIFILE_REL);
#endif
}


#ifdef BUILD_DLL
BOOL setupDll() {
	if (!getBasePathFromFS9(fsBaseDir)) return FALSE;
    setupOtherPathVariables();
    configRead(config, iniFile);
    return TRUE;
}
#else
BOOL setupExe() {
    if (!connectToFSUIPC(FALSE)) return FALSE;
    setupOtherPathVariables();
    configRead(config, iniFile);
    return setupBaseDirVariable();
}
#endif

BOOL setup() {
	char buf[256];

	strcpy(buf,"An error occurred while initializing " PRODUCT ".\r\n");
#ifdef BUILD_DLL
	if (setupDll()) return TRUE;
	strcat(buf,"You can still play Flight Simulator, but " PRODUCT " will not be functional.");
#else
	if (setupExe()) return TRUE;
#endif
	display(DISPLAY_FATAL,buf);
	return FALSE;
}

char* getSceneryCfgName() {
	char* out = malloc(strlen(fsBaseDir)+strlen(SCENERY_CFG)+1);
	strcpy(out, fsBaseDir);
	strcat(out,SCENERY_CFG);
	return out;
}

BOOL setupSceneryInfoMd5() {
	int i;
	MD5* md5 = &(sceneryInfo->sceneryCfgMd5);

	sceneryInfo->layerMd5 = malloc(sceneryInfo->layersCount * sizeof(struct MD5));
	for (i=0; i < sceneryInfo->layersCount; ++i) {
		md5 = malloc(sizeof(struct MD5));
		if (!md5ForFile(md5, sceneryInfo->layerDat[i])) return FALSE;
		sceneryInfo->layerMd5[i] = md5;
	}
	return TRUE;
}

BOOL setupSceneryInfo() {
	sceneryInfo = malloc(sizeof(SceneryInfo));
	sceneryInfo->layersCount = 0;
	sceneryInfo->sceneryCfg = getSceneryCfgName();
	fillSceneryInfo(sceneryInfo);
	display(DISPLAY_DEBUG, "scenery.cfg contains %d valid layers\n", sceneryInfo->layersCount);
	return setupSceneryInfoMd5();
}

void freeSceneryInfo() {
	int i;
	for (i=0; i < sceneryInfo->layersCount; ++i) {
		free(sceneryInfo->layerDat[i]);
		free(sceneryInfo->layerMd5[i]);
	}
	free(sceneryInfo->layerDat);
	free(sceneryInfo->layerMd5);
	free(sceneryInfo->sceneryCfg);
	free(sceneryInfo);
}


int setupAirports() {
	int loadedAirports = 0;

	if (!setupSceneryInfo()) return 0;

	if (!isDebuggedAirportSet()) {
		hDataFile = CreateFile(dataFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hDataFile != INVALID_HANDLE_VALUE) {
			loadedAirports = readDatabase();
			if (loadedAirports != 0) {
				display(DISPLAY_DETAIL, "Successfully loaded %i airport infos from database.", loadedAirports);
				freeSceneryInfo();
				configFreeSections(config);
				return loadedAirports;
			}
		}
	}

	loadedAirports = buildDatabase();
	freeSceneryInfo();
	configFreeSections(config);

	if (loadedAirports != 0) {
		display(DISPLAY_DETAIL, "Successfully imported %i airport infos from scenery.", loadedAirports);
		return loadedAirports;
	}

	display(DISPLAY_FATAL, "No airports in database!");
	return 0;
}

DWORD WINAPI commonMain() {
	ipcMutex = CreateMutexA(NULL, FALSE, NULL);
	if (!setup()) return 1;

	int loadedAirports = setupAirports();
	if (!loadedAirports) return 1;

	/* factor 2 because of artificial entries  */
	airportSearchResults = malloc(sizeof(DWORD)*loadedAirports * 2);

#ifdef BUILD_DLL
	DWORD thread;
	CreateThread(NULL,0, &mainLoop, NULL, 0, &thread);
#else
	SetConsoleCtrlHandler(consoleHandler, TRUE);
	printf("\r\n=============================================================\r\n");
	printf("= autower is running in console mode. Press Ctrl-C to exit. =\r\n");
	printf("=============================================================\r\n\r\n");
	mainLoop(0);
	FSUIPC_Close();
#endif
	return 0;
}

void makeAbsolutePath(char* filename) {
	char buf[MAX_PATH];

	// very simple check, but ought to be sufficient.
	if (strlen(filename) > 1 && filename[1] == ':') {
		return;
	} else if (isUNCFileName(filename)) {
		return;
	}
	else {
		strcpy(buf, fsBaseDir);
		strcat(buf, filename);
		strcpy(filename, buf);
	}
}

BOOL writeDatabaseMd5(MD5* md5) {
	DWORD written;
	if (!WriteFile(hDataFile, md5, sizeof(struct MD5), &written, NULL)) return FALSE;
	return written == sizeof(struct MD5);
}

BOOL writeDatabaseSceneryMd5() {
	int i = 0;
	if (!writeDatabaseMd5(&(sceneryInfo->sceneryCfgMd5))) return FALSE;
	for (i=0; i < sceneryInfo->layersCount; ++i) {
		if (!writeDatabaseMd5(sceneryInfo->layerMd5[i])) return FALSE;
	}
	return TRUE;
}

BOOL openDatabaseForWriting() {
	DWORD written = 0;
	int header = DATFILE_VERSION << 24;
	hDataFile = INVALID_HANDLE_VALUE;

	hDataFile = CreateFile(dataFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	
	if (hDataFile == INVALID_HANDLE_VALUE) {
		LPTSTR err = getFormattedErrorMessage();
		display(DISPLAY_WARN, "Invalid file handle while opening database file %s for writing, error was: %s.", dataFile, err);
		LocalFree(err);
		return FALSE;
	}
	WriteFile(hDataFile,&header,sizeof(header),&written,NULL);
	if (!writeDatabaseSceneryMd5()) return FALSE;
	return TRUE;
}

void writeDatabaseHeaderAndClose(int airports) {
	DWORD written = 0;
	airports |= (DATFILE_VERSION << 24);
	SetFilePointer(hDataFile,0,NULL,FILE_BEGIN);
	WriteFile(hDataFile,&airports,sizeof(airports),&written,NULL);
	CloseHandle(hDataFile);
}

float extractSingleRunwayInfo(AirportInfo* airport, RunwayInfoBuildtime* runway) {
	float apLon = decodeLongitude(airport->towerLongitude);
	float apLat = decodeLatitude(airport->towerLatitude);
	if (apLon == 0) apLon = decodeLongitude(airport->longitude);
	if (apLat == 0) apLat = decodeLatitude(airport->latitude);
	float lonDiff = decodeLongitude(runway->longitude) - apLon;
	float latDiff = decodeLatitude(runway->latitude) -apLat;
	if (lonDiff < 0) lonDiff = -lonDiff;
	if (latDiff < 0) latDiff = -latDiff;

	if (lonDiff < 1.0/5000 && latDiff < 1.0/5000) {
		display(DISPLAY_DEBUG,"lat/lon differences for tower within threshold for single runway center at %s", airport->icao);
		float rwyInfo = (((int)runway->length)*10) + DEGREE_TO_RADIAN(runway->heading);
		//airport->singleRwyInfo = rwyInfo;
		display(DISPLAY_DEBUG, "Single Rwy Info: (%f,%f), len %f m, hdg %f -> combined %f",
			decodeLatitude(runway->latitude), decodeLongitude(runway->longitude),
			runway->length, runway->heading, rwyInfo);
		return rwyInfo;
	}
	return 0.0;
}

void convertRunwayInfoFromBuildToRuntime(AirportInfo* airport) {
	RunwayInfoBuildtime* runway = airport->runwayInfo.runways;
	float singleRunwayInfo = 0.0;
	int runways = 0;
	if (runway != NULL && runway->next == NULL) {
		singleRunwayInfo = extractSingleRunwayInfo(airport, runway);
	}
	while (runway != NULL) {
		RunwayInfoBuildtime* obsolete = runway;
		++runways;
		runway = runway->next;
		free(obsolete);
	}
	airport->runwayInfo.singleRunwayInfo = singleRunwayInfo;
	airport->runways = runways;
}

void freeIcaoListAndWrite() {
	DWORD dummy = 0;
	unsigned int i;
	unsigned int count = getIcaoListSize();
	for (i=0; i < count; ++i) {
		AirportInfo* airport = getAirportInIcaoList(i);
		convertRunwayInfoFromBuildToRuntime(airport);
		if (hDataFile != INVALID_HANDLE_VALUE)
			WriteFile(hDataFile,airport,sizeof(AirportInfo),&dummy,NULL);
		fixupAirport(airport);
		debugAirport(NULL, airport);
	}
	freeIcaoList();
}

int buildDatabase() {
	int i = 0;
	unsigned int airports = 0;

	if (!isDebuggedAirportSet()) openDatabaseForWriting();
	progressCreateWindow();
	progressSetTotalLayersCount(sceneryInfo->layersCount);
	
	/* assign temporary space for the searches during insertion. 10000 should be far enough. */
	airportSearchResults = malloc(sizeof(DWORD)*10000);
	for (i=sceneryInfo->layersCount-1; i >= 0; i--) {
		airports += parseSceneryDat(sceneryInfo->layerDat[i]);
		progressLayerDone();
	}
	free(airportSearchResults);
	freeIcaoListAndWrite();
	if (hDataFile != INVALID_HANDLE_VALUE) {
		writeDatabaseHeaderAndClose(airports);
	}
	progressFinalize();
	return airports;
}

int parseSceneryDatTable(DWORD* table, int count, DWORD base) {
	int i = 0, found = 0;
	char entryFilename[MAX_PATH];

	for (i=0; i < count; ++i) {
		DWORD offset = table[i];
		offset += base;
		SceneryDatEntry *entry = (LPVOID) offset;
		strncpy(entryFilename, entry->filename, entry->filenamelength);
		strcat(entryFilename, ".bgl");
		makeAbsolutePath(entryFilename);
		if (entry->indexedTypes & SCENERY_DAT_INDEX_BIT_AIRPORT) {
			display(DISPLAY_DEBUG, "%s contains airports", entryFilename);
			found += parseAirportsInBglFile(entryFilename);
		} else {
			display(DISPLAY_DEBUG, "%s does not contain airports", entryFilename);
		}
	}

	return found;
}

int parseSceneryDat(char* scenerydat) {
	MappedFile map;
	int found = 0;

	if (!mapFile(scenerydat, &map)) return 0;
	SceneryDatHeader *header = map.base;
	if (header->indexedTypes & SCENERY_DAT_INDEX_BIT_AIRPORT) {
		display(DISPLAY_DEBUG, "%s contains airports\r\n", scenerydat);
		LPVOID start = map.base;
		start += sizeof(struct SceneryDatHeader);
		found = parseSceneryDatTable(start, header->entryCount, (DWORD) map.base);
	} else {
		display(DISPLAY_DEBUG, "%s does not contain airports\r\n", scenerydat);
	}

	unmapFile(&map);
	return found;
}

int parseAirportsInBglFile(LPSTR bglFile) {
	int found = 0;
	MappedFile map;
	
	progressIncrementFileCount(1);

	if (!mapFile(bglFile, &map)) return 0;
	
	found = parseAirportsInBglFileMap(map.base,bglFile);
	unmapFile(&map);
	return found;
}

void parseNameSubrecord(LPVOID subrecord, AirportInfo* airport) {
	BglRecordHeader* header = subrecord;
	DWORD length = header->size - sizeof(struct BglRecordHeader);
	if (length >= sizeof(airport->name)-1) length = sizeof(airport->name)-1;
	memcpy(airport->name,subrecord+sizeof(struct BglRecordHeader),length);
	display(DISPLAY_DEBUG, "Name subrecord yielded airport name: %s", airport->name);
}

void parseComSubrecord(LPVOID subrecord, AirportInfo* airport) {
	int i;
	BglComSubrecord* com = subrecord;
	if (com->type != COM_TYPE_TOWER) {
		display(DISPLAY_DEBUG, "Ignoring COM subrecord because type is %d, not tower (%d)", com->type, COM_TYPE_TOWER);
		return;
	}
	WORD frequency = (com->frequency/1000 - 117000);
	for (i=0; i <= AIRPORT_COM_SIZE; ++i) {
		if (i == AIRPORT_COM_SIZE || airport->com[i] == 0 || airport->com[i] == frequency) break;
	}
	if (i >= AIRPORT_COM_SIZE) {
		display(DISPLAY_WARN, "WARNING: COM #%d *NOT INSERTED* for %s (not enough space)", i, airport->icao);
	}
	else {
		(airport->com)[i] = frequency;
		display(DISPLAY_DEBUG, "Found tower COM #%d at %.3f", i, (double)com->frequency/1000000.0);
	}
}

int findComIndex(AirportInfo* airport, DWORD com) {
	int i;
	for (i=0; i < AIRPORT_COM_SIZE; ++i) {
		if (airport->com[i] == 0) return -1;
		if (airport->com[i] == com) return i;
	}
	return -1;
}

void deleteComAtIndex(AirportInfo* airport, int index) {
	display(DISPLAY_DEBUG, "deleting tower COM at index %u", index);
	while (++index < AIRPORT_COM_SIZE) {
		airport->com[index-1] = airport->com[index];
		if (airport->com[index] == 0) break;
		airport->com[index] = 0;
	}
}

void deleteAllRunways(AirportInfo* airport) {
	display(DISPLAY_DEBUG, "Deleting all runways from %s", airport->icao);
	RunwayInfoBuildtime* current = airport->runwayInfo.runways;
	RunwayInfoBuildtime* obsolete;
	while (current != NULL) {
		obsolete = current;
		current = current->next;
		free(obsolete);
	}
	airport->runwayInfo.runways = NULL;
}

void deleteAllFrequencies(AirportInfo* airport) {
	int i;
	int deleted = 0;
	display(DISPLAY_DEBUG, "deleting all frequencies from %s", airport->icao);
	for (i=0; i < AIRPORT_COM_SIZE; ++i) {
		if (airport->com[i] == 0) break;
		airport->com[i] = 0;
		++deleted;
	}
	display(DISPLAY_DEBUG, "deleted %u frequencies", deleted);
}

void handleDeleteRunwaySubsubrecord(AirportInfo* airport, BglDeleteRunwaySubsubrecord* delete) {
	RunwayInfoBuildtime* current = airport->runwayInfo.runways;
	RunwayInfoBuildtime* last = NULL;
	while (current != NULL) {
		if (current->surface == delete->surface
		&& current->primaryNumber == delete->primaryNumber
		&& current->secondaryNumber == delete->secondaryNumber
		&& current->designators == delete->designators) {
			if (last == NULL) {
				airport->runwayInfo.runways = current->next;
			} else {
				last->next = current->next;
			}
			display(DISPLAY_DEBUG, "Deleted runway %d/%d from %s", current->primaryNumber, current->designators, airport->icao);
			free (current);
			break;
		}
		last = current;
		current = current->next;
	}
}

void handleDeleteFrequencySubsubrecord(AirportInfo *airport, BglDeleteFrequencySubsubrecord *freq) {
    DWORD type = freq->typeAndFrequency >> 28;
    DWORD com = freq->typeAndFrequency & ~(15 << 28);
    com = com / 1000 - 117000;
    display(DISPLAY_DEBUG, "found individual delete request for frequency %.03f of type %d", printableCom(com), type);
    if (type == COM_TYPE_TOWER) {
			int index = findComIndex(airport, com);
			if (index != -1) {
				deleteComAtIndex(airport, index);
			}
		}
}

void parseDeleteSubrecord(LPVOID subrecord, AirportInfo* airport) {
	int displayLevel = DISPLAY_DEBUG;
	int i=0;
	display(displayLevel, "Evaluating DeleteAirport subrecord");
	BglDeleteSubrecord* delete = subrecord;

	if (delete->deleteFlags & DELETE_FLAG_ALL_RUNWAYS) {
		deleteAllRunways(airport);
	}

	if (delete->deleteFlags & DELETE_FLAG_ALL_FREQUENCIES) {
		deleteAllFrequencies(airport);
	};

	if (delete->header.size < sizeof (struct BglDeleteSubrecord)) {
		display(displayLevel, "bailing out because size (%d) is less than DeleteAirport size containing individual runway or frequency deletions (%d)", delete->header.size, sizeof(struct BglDeleteSubrecord));
		return;
	}

	subrecord += sizeof(struct BglDeleteSubrecord);
	for (i=0; i < delete->runwayCount; ++i) {
		handleDeleteRunwaySubsubrecord(airport, subrecord);
		subrecord += sizeof(struct BglDeleteRunwaySubsubrecord);
	}
	for (i=0; i < delete->startCount; ++i) {
		/* not required for us, just skip it */
		subrecord += 4;
	}
	for (i=0; i < delete->frequencyCount; ++i) {
		handleDeleteFrequencySubsubrecord(airport, subrecord);
		subrecord += sizeof(struct BglDeleteFrequencySubsubrecord);
	}
}

char convertRunwayDesignator(BYTE binary) {
	switch (binary) {
	case 1: return 'L';
	case 2: return 'R';
	case 3: return 'C';
	case 4: return 'W';
	default: return ' ';
	}
}

void parseRunwaySubrecord(LPVOID subrecord, AirportInfo* airport) {
	BglRunwaySubrecord* runway = subrecord;
	BOOL closed = (runway->markingFlags & RUNWAY_FLAG_PRIMARY_CLOSED) == RUNWAY_FLAG_PRIMARY_CLOSED;
	closed &= (runway->markingFlags & RUNWAY_FLAG_SECONDARY_CLOSED) == RUNWAY_FLAG_SECONDARY_CLOSED;

	display(DISPLAY_DEBUG, "Found%s runway %d%c/%d%c at (%f,%f), heading %f, length %f meters at %s",
		closed ? " CLOSED" : "",
		runway->primaryNumber, convertRunwayDesignator(runway->primaryDesignator),
		runway->secondaryNumber, convertRunwayDesignator(runway->secondaryDesignator),
		decodeLatitude(runway->latitude), decodeLongitude(runway->longitude), runway->heading, runway->length, airport->icao
	);
	/* Info: with the stock FS9 airports, this ignores roughly 500 runways. */
	if (closed) {
		display (DISPLAY_DETAIL, "runway %d%c at %s ignored because it's closed in both directions.",
				runway->primaryNumber, convertRunwayDesignator(runway->primaryDesignator), airport->icao
		);
		return;
	}
	RunwayInfoBuildtime* out = malloc(sizeof(struct RunwayInfoBuildtime));
	out->surface = runway->surface & 0xFF;
	out->primaryNumber = runway->primaryNumber;
	out->secondaryNumber = runway->secondaryNumber;
	out->designators = runway->secondaryDesignator << 4 | runway->primaryDesignator;
	out->longitude = runway->longitude;
	out->latitude = runway->latitude;
	out->heading = runway->heading;
	out->length = runway->length;

	out->next = airport->runwayInfo.runways;
	airport->runwayInfo.runways = out;
}

void parseSubrecord(char* bglFile, LPVOID base, LPVOID subrecord, AirportInfo* airport, BOOL deleteStage) {
	BglRecordHeader* header = subrecord;
	switch (header->type) {
	case SUBRECORD_TYPE_COM:
		if (!deleteStage) parseComSubrecord(subrecord, airport);
		break;
	case SUBRECORD_TYPE_NAME:
		if (!deleteStage) parseNameSubrecord(subrecord, airport);
		break;
	case SUBRECORD_TYPE_RUNWAY:
		if (!deleteStage) parseRunwaySubrecord(subrecord, airport);
		break;
	case SUBRECORD_TYPE_DELETE:
		if (deleteStage) parseDeleteSubrecord(subrecord, airport);
	default:
		break;
	}
}

void parseSubrecords(char* bglFile, LPVOID base, BglAirportRecord* input, AirportInfo* output, BOOL deleteStage) {
	DWORD offset = (DWORD)input + sizeof(struct BglAirportRecord);
	DWORD limit = (DWORD)input + input->header.size;
	while (offset < limit) {
		BglRecordHeader* header = (BglRecordHeader*) offset;
		display(DISPLAY_DEBUG, "airport subrecord at offset %08X is of type %04X, size %d",
				offset - (DWORD)base, header->type, header->size);
		parseSubrecord(bglFile, base, header, output, deleteStage);
		offset += header->size;
	}
	if (offset != limit) {
		display(DISPLAY_WARN, "%s: airport record at offset %08X specified a size of %d, but used %d",
				bglFile,
				(DWORD)input,
				input->header.size,
				offset - (DWORD)input
		);
	}
}

AirportInfo* createBasicAirportInfoFromBglRecord(BglAirportRecord* record) {
	AirportInfo *airport = malloc(sizeof(AirportInfo));
	memset(airport,0,sizeof(AirportInfo));
	airport->runways = record->runwaySubrecords;
	airport->latitude = record->airportLatitude;
	airport->longitude = record->airportLongitude;
	airport->altitude = record->airportAltitude;
	airport->towerLatitude = record->towerLatitude;
	airport->towerLongitude = record->towerLongitude;
	airport->towerAltitude = record->towerAltitude;
	convertIcao(record->icao, airport->icao);
	return airport;
}

int parseAirportRecord(char* bglFile, LPVOID base, DWORD offset) {
	BOOL isOverriding = FALSE;
	BglAirportRecord* record = base + offset;
	AirportInfo* airport = createBasicAirportInfoFromBglRecord(record);

	if (airport->towerAltitude != 0 && decodeAltitude(airport->towerAltitude) <= -1000) {
		debugAirport(bglFile, airport);
		int level = DISPLAY_DETAIL;
		if (isDebuggedAirport(airport)) {
			level = DISPLAY_ALWAYS;
		}
		display(level, "%s IGNORED because tower altitude %f <= -1000 m ASL",
				airport->icao, decodeAltitude(airport->towerAltitude));
		free(airport);
		return 0;
	}
	if (airportsTree != NULL) {
		AirportInfo* existing = findOverriddenAirport(airport);
		if (existing != NULL) {
			mergeTowerInfos(existing, airport);
			isOverriding = TRUE;
			free(airport);
			airport = existing;
		}
	}

	if (isOverriding) {
		/* handle delete subrecords first */
		parseSubrecords(bglFile, base, record, airport, TRUE);
	}

	/* add information from other subrecords */
	parseSubrecords(bglFile, base, record, airport, FALSE);

	debugAirport(bglFile, airport);

	if (!isOverriding) {
		insertAirportInKdTree(airport, DISPLAY_DETAIL);
		insertAirportInIcaoList(airport);
		progressIncrementAirportCount(1);
		return 1;
	}
	return 0;
}

int parseRecord(char* bglFile, LPVOID base, DWORD offset) {
	BglRecordHeader* header = base + offset;
	if (header->type != RECORD_TYPE_AIRPORT) {
		display(DISPLAY_DETAIL, "File %s, offset %08X: expected airport ID (%04X), but found %04X. Ignoring this record.",
				bglFile, offset, RECORD_TYPE_AIRPORT, header->type
		);
		return 0;
	}
	return parseAirportRecord(bglFile, base, offset);
}

int parseRecords(char* bglFile, LPVOID base, DWORD subsectionOffset, DWORD records, DWORD size) {
	int airports = 0;
	int record = 0;
	DWORD offset = subsectionOffset;
	for (record=0; record < records; ++record) {
		BglRecordHeader* header = base + offset;
		display(DISPLAY_DEBUG, "record %d at offset %08X is of type %04X, size %d", record, offset, header->type, header->size);
		airports += parseRecord(bglFile, base, offset);
		offset += header->size;
		if (offset > subsectionOffset + size) {
			display(DISPLAY_WARN,"While reading record %d of %d in file %s: ended up at offset 0x%08X > 0x%08X + 0x%X (subsection start + subsection size)",
					record, records, bglFile, offset, subsectionOffset, size
			);
			break;
		}
	}
	return airports;
}

int parseAirportSubsection(char* bglFile, LPVOID base, DWORD offset, DWORD subsections) {
	int subsection = 0;
	int airports = 0;
	for (subsection = 0; subsection < subsections; ++subsection) {
		BglSubsectionPointer* pointer = base + offset + subsection * sizeof(struct BglSubsectionPointer);
		display(DISPLAY_DEBUG, "subsection %d at offset %08X has %d records, size %d", subsection, pointer->offset, pointer->records, pointer->size);
		airports += parseRecords(bglFile, base, pointer->offset, pointer->records, pointer->size);
	}
	return airports;
}

int parseSectionPointers(char* bglFile, BglFileHeader* fileHeader) {
	LPVOID base = fileHeader;
	int section = 0;
	int airports = 0;
	for (section = 0; section < fileHeader->sections; ++section) {
		BglSectionPointer* pointer = base + fileHeader->headerSize + section * sizeof(BglSectionPointer);
		if (pointer->type == SECTION_TYPE_AIRPORT) {
			display(DISPLAY_DEBUG, "parsing section %d of type %08X, offset %08X, as airport subsection", section, pointer->type, pointer->offset);
			airports += parseAirportSubsection(bglFile, base, pointer->offset, pointer->subsections);
		} else {
			display(DISPLAY_DEBUG, "ignoring section %d of type %08X", section, pointer->type);
		}
	}
	return airports;
}

int parseAirportsInBglFileMap(LPVOID fileMap, LPSTR bglFile) {
	BglFileHeader* header = fileMap;
	if (header->magic != 0x19920201) {
		display(DISPLAY_WARN, "Unknown magic %08X in file %s, aborting parse", header->magic, bglFile);
		return 0;
	}
	if (header->headerSize != sizeof (struct BglFileHeader)) {
		display(DISPLAY_WARN, "Header size in %s is %d, expected %d. Continuing nevertheless.", bglFile, header->headerSize, sizeof(struct BglFileHeader));
	}

	display(DISPLAY_DEBUG, "%d sections in file", header->sections);
	return parseSectionPointers(bglFile, header);
}	


void convertIcao(DWORD code, char* output) {
	int position = 0;
	int letter = 0;
	
	code /= 0x20;
	for (position=4; position >= 0; position--) {
		letter = code % 38;
		code /= 38;
		if (letter > 11) {
			letter = letter - 12 + 0x41;
		}
		else if (letter >= 2) {
			letter = letter -2 + 0x30;
		}
		else { //if (ci == 1) {
			letter = 0x20;
		}
		output[position] = letter;
		if (letter == 0) break;
	}
	
	/* shift left the unused spaces */
	letter = 0;
	for (position=0; position < 5; position++) {
		if (output[position] == 0x20) letter++;
		else break;
	}
	for (position=0; position < 6 - letter; position++) {
		output[position] = output[letter+position];
	}
}

void dumpAirportInfo(AirportInfo *airport, int debugLevel) {
	if (airport == NULL) {
		display(debugLevel, "airport == NULL");
		return;
	}
	double lat = decodeLatitude(airport->latitude);
	double lon = decodeLongitude(airport->longitude);
	double alt = decodeAltitude(airport->altitude);

	double tlat = decodeLatitude(airport->towerLatitude);
	double tlon = decodeLongitude(airport->towerLongitude);
	double talt = decodeAltitude(airport->towerAltitude);

	display(debugLevel, "%s\t%s: AP @(%f, %f, %f), %d rwys (%s); TWR @(%f, %f, %f) COM @ {%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f} dst %f nm",
		airport->icao,
		airport->name,
		lat, lon, alt, airport->runways,
		airport->runwayInfo.singleRunwayInfo == 0 ? "nf" : "F",
		tlat,tlon,talt,
		printableCom(airport->com[0]),
		printableCom(airport->com[1]),
		printableCom(airport->com[2]),
		printableCom(airport->com[3]),
		printableCom(airport->com[4]),
		printableCom(airport->com[5]),
		printableCom(airport->com[6]),
		printableCom(airport->com[7]),
		airport->currentDistance);
}
