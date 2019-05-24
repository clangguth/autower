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

#ifndef LINKAGE_H_
#define LINKAGE_H_

typedef int ID;
typedef unsigned int VAR32;
typedef VAR32 FLAGS32;
#define   FSAPI __stdcall

/* These are the two structs that are exported from the DLL */

typedef struct FSModule {
    ID module;
    void** functions;
} FSModule;

#define IMPORT_FS9_ID 1
#define IMPORT_FS9 0
#define IMPORT_FSUIPC_ID 0x505A
#define IMPORT_FSUIPC 1
#define IMPORT_COUNT 2

typedef struct IMPORTTABLE
{
	FSModule modules[IMPORT_COUNT+1];
} IMPORTTABLE;
__declspec (dllexport) IMPORTTABLE ImportTable;

typedef struct LINKAGE
{
    ID ModuleID;
    void (FSAPI *ModuleInit) ();
    void (FSAPI *ModuleDeInit) ();
    FLAGS32 ModuleFlags;
    UINT32 ModulePriority;
    UINT32 ModuleVersion;
    LPVOID* Table[];
} LINKAGE;
__declspec (dllexport) LINKAGE Linkage;

#endif /* LINKAGE_H_ */
