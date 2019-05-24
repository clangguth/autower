/*
 * linkage.h
 *
 *  Created on: 15.05.2010
 *      Author: root
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
