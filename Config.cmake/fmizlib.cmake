#    Copyright (C) 2023 Modelon AB
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.
#
#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(FMILIB_USE_NATIVE_ZLIB)

set(ZLIB_USE_STATIC_LIBS ON)
include(FindZLIB)

find_package(ZLIB)
if(NOT ZLIB_FOUND)
    message(FATAL_ERROR
        "Could not locate native ZLIB, try setting ZLIB_LIBRARY & ZLIB_INCLUDE_DIR,
        or consider building without the FMILIB_USE_NATIVE_ZLIB option."
    )
endif(NOT ZLIB_FOUND)

add_library(zlib STATIC IMPORTED ZLIB::ZLIB)
## ZLIB_INCLUDE_DIRS set by FindZLIB
set_target_properties(
    zlib PROPERTIES
    IMPORTED_LOCATION "${ZLIB_LIBRARIES}"
    INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
)

else(FMILIB_USE_NATIVE_ZLIB) # use zlib from ThirdParty/zlib

if(NOT ZLIB_PREFIX)
set(ZLIB_PREFIX "${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.2.13")
set(ZLIB_SOURCE "${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.2.13")
set(ZLIB_INSTALL "${FMILIBRARYBUILD}/zlibext/install")

include(ExternalProject)
ExternalProject_Add(
    zlibext
    PREFIX      "${ZLIB_PREFIX}"
    SOURCE_DIR  "${ZLIB_SOURCE}"
    BINARY_DIR  "${FMILIBRARYBUILD}/zlibext"
    INSTALL_DIR "${ZLIB_INSTALL}"
    TMP_DIR     "${FMILIBRARYBUILD}/zlibext/temp"
    STAMP_DIR   "${FMILIBRARYBUILD}/zlibext/stamp"
)

set(ZLIB_SETTINGS
    -DBUILD_SHARED_LIBS:BOOLEAN=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOLEAN=ON
    -DSKIP_INSTALL_LIBRARIES:BOOLEAN=NOT FMILIB_INSTALL_SUBLIBS
    -DSKIP_INSTALL_FILES:BOOLEAN=ON
    -DSKIP_INSTALL_HEADERS:BOOLEAN=OFF
    -DSKIP_INSTALL_ALL:BOOLEAN=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${FMILIBRARYBUILD}/zlibext/install
)

ExternalProject_Add_Step(
    zlibext dependent_reconfigure
    DEPENDEES configure
    DEPENDERS build
    COMMAND ${CMAKE_COMMAND} -E echo "Running:  ${CMAKE_COMMAND} -G \"${CMAKE_GENERATOR}\"  ${ZLIB_SETTINGS} ${ZLIB_SOURCE}"
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${ZLIB_SETTINGS} "${ZLIB_SOURCE}"
    DEPENDS ${CMAKE_BINARY_DIR}/CMakeCache.txt
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlibext
)

# Fixes for zlib naming irregularities based on platform
if(WIN32)
    set(ZLIB_LIB_SUFFIX lib)
endif()
set(ZLIB_LIB_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})

if("${CMAKE_CFG_INTDIR}" STREQUAL ".")
    set(zlib_lib "${CMAKE_BINARY_DIR}/zlibext/${ZLIB_LIB_PREFIX}z${ZLIB_LIB_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(zlib_lib "${CMAKE_BINARY_DIR}/zlibext/${CMAKE_CFG_INTDIR}/${ZLIB_LIB_PREFIX}z${ZLIB_LIB_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

add_custom_command(
    OUTPUT "${zlib_lib}"
    DEPENDS zlibext
)
add_custom_target(tmp_zlib DEPENDS ${zlib_lib})

add_library(zlib STATIC IMPORTED)
set(ZLIB_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/zlibext/install/include")
set_target_properties(
    zlib PROPERTIES
    IMPORTED_LOCATION "${zlib_lib}"
    INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
    )

add_dependencies(zlib tmp_zlib)
    
if(FMILIB_INSTALL_SUBLIBS)
    install(FILES "${zlib_lib}" DESTINATION lib)
endif()

endif(NOT ZLIB_PREFIX)
endif(FMILIB_USE_NATIVE_ZLIB)
