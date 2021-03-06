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

#include "config.h"
#include "display.h"
#include "inifile.h"
#include <windows.h>
#include <stdio.h>

Configuration* configInitialize() {
	Configuration* config = malloc( sizeof (struct Configuration));

	config->displayLevel = DISPLAY_DEFAULT;
	config->maxDisplayInfoAltitude = DEFAULT_MAXDISPLAYINFOALTITUDE;
	config->towerAlt1Rw = DEFAULT_TOWERALT1RW;
	config->towerAlt2Rw = DEFAULT_TOWERALT2RW;
	config->towerAlt3Rw = DEFAULT_TOWERALT3RW;
	config->towerAlt4Rw = DEFAULT_TOWERALT4RW;
	config->towerAlt5Rw = DEFAULT_TOWERALT5RW;
	config->towerAlt6Rw = DEFAULT_TOWERALT6RW;
	config->towerMinCandidates = DEFAULT_TOWERMINCANDIDATES;
	config->comLockThreshold = DEFAULT_COMLOCKTHRESHOLD;
	config->zoomLevel = DEFAULT_ZOOMLEVEL;
	config->updateInterval = DEFAULT_UPDATEINTERVAL;
	config->simpleFixHeading = DEFAULT_SIMPLEFIXHEADING;
	config->simpleFixDistance = DEFAULT_SIMPLEFIXDISTANCE;
	config->displayDuration = DEFAULT_DISPLAYDURATION;
	config->alwaysSetTower = DEFAULT_ALWAYSSETTOWER;
	config->forceFsPathToUNC = DEFAULT_FORCEFSPATHTOUNC;
	config->integratedUI = DEFAULT_INTEGRATEDUI;
	config->progressDisplayDelay = DEFAULT_PROGRESSDISPLAYDELAY;

	setDisplayLevel(config->displayLevel);
	return config;
}

/**
 * This enumerates all sections found in the .ini file into
 * config->sections. (If the configuration is not found or some
 * other error occurs, sections will be NULL).
 * The sections list will contain a space-separated list of all
 * sections present in the ini file.
 *
 * The sections list should be freed once it is no longer needed, by
 * calling configFreeSections.
 */
void configEnumerateSections(Configuration* config, char* iniFile) {
	config->sections = iniFileEnumerateSections(iniFile);
}

void configFreeSections(Configuration* config) {
	if (config->sections != NULL) {
		free (config->sections);
	}
}

BOOL configContainsICAOSection(Configuration* config, char* icao) {
	if (config == NULL || config->sections == NULL || icao == NULL) {
		return FALSE;
	}
	char* lookFor = malloc(strlen(icao)+3);
	if (lookFor == NULL) {
		return FALSE;
	}

	lookFor[0] = ' ';
	strcpy(&lookFor[1], icao);
	strcat(lookFor, " ");
	return strstr(config->sections, lookFor) != NULL;
}

void configRead(Configuration* config, char* iniFile) {
    config->displayLevel = GetPrivateProfileInt(INI_SECTIONKEY,KEY_LOGLEVEL,config->displayLevel,iniFile);
    config->displayDuration = GetPrivateProfileInt(INI_SECTIONKEY,KEY_DISPLAYDURATION,config->displayDuration,iniFile);
    config->maxDisplayInfoAltitude = GetPrivateProfileInt(INI_SECTIONKEY,KEY_MAXDISPLAYINFOALTITUDE,config->maxDisplayInfoAltitude,iniFile);
    config->towerMinCandidates = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERMINCANDIDATES,config->towerMinCandidates,iniFile);
    config->comLockThreshold = GetPrivateProfileInt(INI_SECTIONKEY,KEY_COMLOCKTHRESHOLD,config->comLockThreshold,iniFile);
    config->zoomLevel = GetPrivateProfileInt(INI_SECTIONKEY,KEY_ZOOMLEVEL,config->zoomLevel,iniFile);
    config->updateInterval = GetPrivateProfileInt(INI_SECTIONKEY,KEY_UPDATEINTERVAL,config->updateInterval,iniFile);
    config->towerAlt1Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT1RW,config->towerAlt1Rw,iniFile);
    config->towerAlt2Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT2RW,config->towerAlt2Rw,iniFile);
    config->towerAlt3Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT3RW,config->towerAlt3Rw,iniFile);
    config->towerAlt4Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT4RW,config->towerAlt4Rw,iniFile);
    config->towerAlt5Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT5RW,config->towerAlt5Rw,iniFile);
    config->towerAlt6Rw = GetPrivateProfileInt(INI_SECTIONKEY,KEY_TOWERALT6RW,config->towerAlt6Rw,iniFile);
    config->simpleFixHeading = GetPrivateProfileInt(INI_SECTIONKEY,KEY_SIMPLEFIXHEADING,config->simpleFixHeading,iniFile);
    config->simpleFixDistance = GetPrivateProfileInt(INI_SECTIONKEY,KEY_SIMPLEFIXDISTANCE,config->simpleFixDistance,iniFile);
    config->alwaysSetTower = GetPrivateProfileInt(INI_SECTIONKEY,KEY_ALWAYSSETTOWER,config->alwaysSetTower,iniFile);
    config->forceFsPathToUNC = GetPrivateProfileInt(INI_SECTIONKEY,KEY_FORCEFSPATHTOUNC,config->forceFsPathToUNC,iniFile);
    config->integratedUI = GetPrivateProfileInt(INI_SECTIONKEY,KEY_INTEGRATEDUI,config->integratedUI,iniFile);
    config->progressDisplayDelay = GetPrivateProfileInt(INI_SECTIONKEY,KEY_PROGRESSDISPLAYDELAY,config->progressDisplayDelay,iniFile);


	setDisplayLevel(config->displayLevel);
	configEnumerateSections(config, iniFile);

	display(DISPLAY_DETAIL, "Configuration dump:\r\n"
			"logLevel=%u\r\n"
			"displayDuration=%d\r\n"
			"maxDisplayInfoAltitude=%ld\r\n"
			"towerMinCandidates=%u\r\n"
			"comLockThreshold=%u\r\n"
			"zoomLevel=%u\r\n"
			"updateInterval=%u\r\n"
			"towerAlt1Rw=%u\r\ntowerAlt2Rw=%u\r\ntowerAlt3Rw=%u\r\ntowerAlt4Rw=%u\r\ntowerAlt5Rw=%u\r\ntowerAlt6Rw=%u\r\n"
			"simpleFixHeading=%u\r\n"
			"simpleFixDistance=%d\r\n"
			"alwaysSetTower=%d\r\n"
			"forceFsPathToUNC=%u\r\n"
			"integratedUI=%u\r\n"
			"progressDisplayDelay=%u\r\n",
			config->displayLevel, config->displayDuration, config->maxDisplayInfoAltitude, config->towerMinCandidates,
			config->comLockThreshold, config->zoomLevel, config->updateInterval,
			config->towerAlt1Rw, config->towerAlt2Rw, config->towerAlt3Rw, config->towerAlt4Rw, config->towerAlt5Rw, config->towerAlt6Rw,
			config->simpleFixHeading, config->simpleFixDistance, config->alwaysSetTower, config->forceFsPathToUNC,
			config->integratedUI, config->progressDisplayDelay
	);
}
