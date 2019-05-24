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
#include <string.h>
#include "scenerycfg.h"
#include "autower.h"
#include "display.h"
#include "inifile.h"

#define MAX_AREA_LENGTH (16)
#define TITLE_LENGTH (64)

typedef struct SceneryLayer {
//	char* section;
	int number;
	char* path;
	struct SceneryLayer* next;
	char title[TITLE_LENGTH];
} SceneryLayer;

BOOL verifyAndFixSceneryDat(char* path) {
	int debugLevel = DISPLAY_DEBUG;
	char alternativePath[MAX_PATH];

	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return TRUE;
	}
	display(debugLevel, "scenery.dat not found at %s", path);
	/* maybe the path is one level too deep: "scenery\scenery.dat" */
	if (strlen(path) < 20) return FALSE;
	strcpy(alternativePath, path);
	char* alternativeStart = alternativePath + strlen(path) - 19;
	strcpy(alternativeStart, SCENERY_DAT);
	display(debugLevel, "applying quirk to look for it at %s", alternativePath);
	hFile = CreateFile(alternativePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		strcpy(path, alternativePath);
		return TRUE;
	}
	display(debugLevel, "File still wasn't found, giving up for this file.");
	return FALSE;
}

SceneryLayer* mergeLayers(SceneryLayer* head, SceneryLayer* new) {
	/* layers will normally come in ascending order (in terms of number)
	 * so we keep the list sorted with the highest element at the head.
	 * New layers with an existing number should be *pre*pended to the
	 * existing layer.
	 */
	SceneryLayer* current = head;

	if (current == NULL || current->number <= new->number) {
		// new head of list
		new->next = current;
		return new;
	}
	while (TRUE) {
		if (current->next == NULL || current->next->number <= new->number) {
			new->next = current->next;
			current->next = new;
			break;
		}
		current = current->next;
	}
	return head;
}

SceneryLayer* updateSceneryLayers(int* count, SceneryLayer* layers, char* section, char* cfg) {
	char buf[MAX_PATH];

	GetPrivateProfileString(section,"active","true",buf,sizeof(buf),cfg);
	if (!strcmpi(buf, "false")) {
		display(DISPLAY_DETAIL, "%s is set as inactive, ignoring it", section);
		return layers;
	}

	GetPrivateProfileString(section, "Local","",buf,sizeof(buf),cfg);
	if (strlen(buf) == 0) {
		GetPrivateProfileString(section,"Remote","",buf,sizeof(buf),cfg);
		if (strlen(buf) == 0)  {
			display(DISPLAY_INFO, "neither local nor remote path is set for %s, ignoring area", section);
			return layers;
		}
	}

	makeAbsolutePath(buf);
	if (buf[strlen(buf)-1] != '\\') {
		strcat(buf, "\\");
	}
	strcat(buf, SCENERY_DAT);

	// ok, we're getting serious now, as the entry seems valid.
	SceneryLayer* thisLayer = malloc(sizeof(struct SceneryLayer));
	thisLayer->number = GetPrivateProfileInt(section, "Layer", 0, cfg);
	GetPrivateProfileString(section, "Title", section, thisLayer->title ,TITLE_LENGTH, cfg);

	if (verifyAndFixSceneryDat(buf)) {
		thisLayer->path = malloc(strlen(buf)+1);
		strcpy(thisLayer->path, buf);
	} else {
		display(DISPLAY_INFO, "%s: %s could not be opened for reading. Ignoring that layer!", section, buf);
		free(thisLayer);
		return layers;
	}
	display(DISPLAY_DEBUG, "%s (%s) is being added as layer %d, path %s", section, thisLayer->title, thisLayer->number, thisLayer->path);
	++*count;
	return mergeLayers(layers, thisLayer);
}

void fillSceneryInfo(SceneryInfo* info) {
	char* sections = iniFileEnumerateSections(info->sceneryCfg);
	if (sections == NULL) return;

	SceneryLayer* layers = NULL;

	char area[MAX_AREA_LENGTH];
	char match[MAX_AREA_LENGTH];

	char* start = sections;
	while (*start != '\0') {
		if (*start == ' ') {
			++start; continue;
		}
		// start is set, now find end
		char* end = start+1;
		while (*end != '\0' && *end != ' ') {
			++end;
		}
		// end is found as well
		strncpy(area, start, MAX_AREA_LENGTH);
		area[((end - start >= MAX_AREA_LENGTH) ? MAX_AREA_LENGTH-1 : (end - start))] = '\0';

		strncpy(match, area, MAX_AREA_LENGTH);
		CharLower(match);
		start = end;
		if (strstr(match, "area.") == match) {
			display(DISPLAY_DEBUG, "Found area section: %s", match);
			layers = updateSceneryLayers(&(info->layersCount), layers, area, info->sceneryCfg);
		}
	}

	// everything parsed, now convert the stuff and free temporary resources
	int index = 0;
	info->layerDat = malloc(sizeof(char*) * info->layersCount);
	SceneryLayer* obsolete = layers;

	display(DISPLAY_DEBUG, "Found %d usable areas, displayed in FS priority order:", info->layersCount);
	while (obsolete != NULL) {
		display(DISPLAY_DEBUG, "#%d %s", obsolete->number, obsolete->title);
		obsolete = obsolete->next;
	}

	for (index = 0; index < info->layersCount; ++index) {
		info->layerDat[index] = layers->path;
		obsolete = layers;
		layers = layers->next;
		free(obsolete);
	}
}
