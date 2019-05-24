cmake_minimum_required(VERSION 3.14)
project(autower C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

link_libraries(psapi comctl32)

add_definitions(-D_ATT_SYNTAX)

if(CMAKE_ARTIFACT_TYPE MATCHES EXE)
    message("Artifact will be: autower.exe")

    add_definitions(-DUSE_ICON)

    add_executable(autower
            src/autower.c
            src/autower.h
            src/bgl.h
            src/config.c
            src/config.h
            src/coords.c
            src/coords.h
            src/display.c
            src/display.h
            src/FSUIPC_User.h
            src/fsui.c
            src/fsui.h
            src/icaolist.c
            src/icaolist.h
            src/inifile.c
            src/inifile.h
            src/linkage.h
            src/metadata.rc
            src/product.h
            src/IPCuser.c
            src/IPCuser.h
            src/kdtree.c
            src/md5.c
            src/md5.h
            src/md5Usage.c
            src/md5Usage.h
            src/memmap.c
            src/memmap.h
            src/progressWindow.c
            src/progressWindow.h
            src/progressWindowBasic.c
            src/progressWindowBasic.h
            src/progressWindowFS.c
            src/progressWindowFS.h
            src/scenerycfg.c
            src/scenerycfg.h
            )

    if(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
        if(EXISTS ../upx.exe)
            message("Found upx.exe, release build of autower.exe will be packed.")
            add_custom_command(TARGET autower
                    COMMAND ../upx.exe --best -i --overlay=strip --compress-icons=0 autower.exe
                    COMMENT "Running upx on .exe"
                    )
        else()
            message("upx.exe not found, release build of autower.dll will not be packed.")
        endif()
    endif()

elseif(CMAKE_ARTIFACT_TYPE MATCHES DLL)
    message("Artifact will be: autower.dll")
    add_definitions(-DBUILD_DLL)

    add_library(autowerdll SHARED
            src/autower.c
            src/autower.h
            src/bgl.h
            src/config.c
            src/config.h
            src/coords.c
            src/coords.h
            src/display.c
            src/display.h
            src/FSUIPC_User.h
            src/fsui.c
            src/fsui.h
            src/icaolist.c
            src/icaolist.h
            src/inifile.c
            src/inifile.h
            src/linkage.h
            src/metadata.rc
            src/product.h
            src/IPCuser.c
            src/IPCuser.h
            src/kdtree.c
            src/md5.c
            src/md5.h
            src/md5Usage.c
            src/md5Usage.h
            src/memmap.c
            src/memmap.h
            src/progressWindow.c
            src/progressWindow.h
            src/progressWindowBasic.c
            src/progressWindowBasic.h
            src/progressWindowFS.c
            src/progressWindowFS.h
            src/scenerycfg.c
            src/scenerycfg.h
            )

    # rename DLL output to autower.dll
    set_target_properties(autowerdll PROPERTIES PREFIX "")
    set_target_properties(autowerdll PROPERTIES OUTPUT_NAME "autower")

    if(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
        if(EXISTS ../upx.exe)
            message("Found upx.exe, release build of autower.dll will be packed.")
            add_custom_command(TARGET autowerdll
                    COMMAND ../upx.exe --best -i --overlay=strip --compress-icons=0 autower.dll
                    COMMENT "Running upx on .dll"
                    )
        else()
            message("upx.exe not found, release build of autower.dll will not be packed.")
        endif()
    endif()
else()
    message("Unknown artifact type -- please specify DMAKE_ARTIFACT_TYPE as EXE or DLL")
endif()
