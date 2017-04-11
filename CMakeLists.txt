#----------INITILIZATION PHASE----------

#set minimum version of cmake for compiling this project
cmake_minimum_required(VERSION 3.5)
#set project name
project(MiKABoO)
#set uarm directory path
set(uarmDir /usr/include/uarm)
#set C compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
#set C compiler flags
set(CMAKE_C_FLAGS "-mcpu=arm7tdmi -D UARM_MACHINE_COMPILING=1")
#set linker command. <OBJECTS> are executable binaries. <LINK_LIBRARIES> are dependancies binaries.
set(CMAKE_C_LINK_EXECUTABLE "arm-none-eabi-ld <LINK_FLAGS> -o <TARGET> <OBJECTS> <LINK_LIBRARIES> ")
#set linker flags
set(CMAKE_EXE_LINKER_FLAGS "-T ${uarmDir}/ldscripts/elf32ltsarm.h.uarmcore.x")
#set build type as debug
set(CMAKE_BUILD_TYPE Debug)
#enable including currect directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#include uarm directory
include_directories(${uarmDir})

#----------DECLARING SOURCES PHASE----------

#declaring sources for data structured needed for both phase 1 and phase 2
set(dataStructures
        pcbImpl.c
        tcbImpl.c
        msgqImpl.c)
#declaring phase 1 sources
set(phase1Sources
        p1test.c
        ${dataStructures}
        )
#declaring phase 2 sources
set(phase2Sources
        init.c
        ${dataStructures}
        scheduler.c
        p2test.c
        exception.c
        interrupt.c
        ssi.c
        utils.c)

#declaring path for uarm libraries
set(libuarm ${uarmDir}/libuarm.o)
set(libdiv ${uarmDir}/libdiv.o)
set(crtso ${uarmDir}/crtso.o)
#specify C language for linking library object file
set_source_files_properties(${libuarm} $(libdiv) ${crtso} PROPERTIES LINKER_LANGUAGE C EXTERNAL_OBJECT TRUE)

#----------FINAL TARGETS PHASE----------

#create executable for phase 1
add_executable(test1  ${phase1Sources} ${crtso} ${libuarm})

#create executable for phase 2
add_executable(test2 ${phase2Sources} ${crtso} ${libuarm} ${libdiv})