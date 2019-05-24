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

#ifndef SCENERYCFG_H_
#define SCENERYCFG_H_

#define SCENERY_DAT "scenery.dat"
#define SCENERY_CFG "scenery.cfg"

#include "md5Usage.h"

#pragma pack(1)
typedef struct SceneryInfo {
	char* sceneryCfg;
	MD5 sceneryCfgMd5;
	int layersCount;
	char** layerDat;
	MD5** layerMd5;
} SceneryInfo, *PSceneryInfo;

void fillSceneryInfo(SceneryInfo* sceneryInfo);


#endif /* SCENERYCFG_H_ */
