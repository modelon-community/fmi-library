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

if(FMILIB_FIND_PACKAGE_ZLIB)

    set(ZLIB_USE_STATIC_LIBS ON)
    include(FindZLIB)

    find_package(ZLIB)
    if(NOT ZLIB_FOUND)
        message(FATAL_ERROR "Could not locate ZLIB using find_package.")
    endif()

    add_library(zlib STATIC IMPORTED ZLIB::ZLIB)
    ## ZLIB_INCLUDE_DIRS set by FindZLIB
    set_target_properties(
        zlib PROPERTIES
        IMPORTED_LOCATION "${ZLIB_LIBRARIES}"
        INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
    )

else() # build zlib from ThirdParty/zlib

    set(ZLIB_PREFIX "${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.3.1")
    set(ZLIB_SOURCE_DIR "${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.3.1")
    set(ZLIB_INSTALL_DIR "${CMAKE_BINARY_DIR}/zlibext/install")

    include(ExternalProject)
    ExternalProject_Add(
        zlibext
        PREFIX      "${ZLIB_PREFIX}"
        SOURCE_DIR  "${ZLIB_SOURCE_DIR}"
        BINARY_DIR  "${CMAKE_BINARY_DIR}/zlibext"
        INSTALL_DIR "${ZLIB_INSTALL_DIR}"
        TMP_DIR     "${CMAKE_BINARY_DIR}/zlibext/temp"
        STAMP_DIR   "${CMAKE_BINARY_DIR}/zlibext/stamp"
    )

    set(ZLIB_SETTINGS
        -DBUILD_SHARED_LIBS:BOOLEAN=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOLEAN=ON
        -DSKIP_INSTALL_LIBRARIES:BOOLEAN=NOT FMILIB_INSTALL_SUBLIBS
        -DSKIP_INSTALL_FILES:BOOLEAN=ON
        -DSKIP_INSTALL_HEADERS:BOOLEAN=OFF
        -DSKIP_INSTALL_ALL:BOOLEAN=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/zlibext/install
    )

    ExternalProject_Add_Step(
        zlibext dependent_reconfigure
        DEPENDEES configure
        DEPENDERS build
        COMMAND ${CMAKE_COMMAND} -E echo "Running:  ${CMAKE_COMMAND} -G \"${CMAKE_GENERATOR}\"  ${ZLIB_SETTINGS} ${ZLIB_SOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${ZLIB_SETTINGS} "${ZLIB_SOURCE_DIR}"
         # COMMAND: Cleans up changes to the source dir made by the zlib build system.
        COMMAND ${CMAKE_COMMAND} -E rename ${ZLIB_SOURCE_DIR}/zconf.h.included ${ZLIB_SOURCE_DIR}/zconf.h
        DEPENDS ${CMAKE_BINARY_DIR}/CMakeCache.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlibext
    )

    # Fixes for zlib naming irregularities based on platform
    if(WIN32)
        set(ZLIB_LIB_SUFFIX lib)
    endif()
    set(ZLIB_LIB_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})


    if("${CMAKE_CFG_INTDIR}" STREQUAL ".")
        set(ZLIB_LIBDIR "${CMAKE_BINARY_DIR}/zlibext")
        set(ZLIB_DEBUG_POSTFIX "")  # Using CMAKE_DEBUG_POSTFIX doesn't seem to work reliably
    else()
        set(ZLIB_LIBDIR "${CMAKE_BINARY_DIR}/zlibext/${CMAKE_CFG_INTDIR}")
        set(ZLIB_DEBUG_POSTFIX d)
    endif()
    set(zlib_lib       "${ZLIB_LIBDIR}/${ZLIB_LIB_PREFIX}z${ZLIB_LIB_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(zlib_lib_debug "${ZLIB_LIBDIR}/${ZLIB_LIB_PREFIX}z${ZLIB_LIB_SUFFIX}${ZLIB_DEBUG_POSTFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    add_custom_command(
        # OUTPUT: Workaround to make it explicit that target 'zlib' produces 'zlib_lib'. (Ninja complains otherwise.)
        OUTPUT "${zlib_lib}"
        DEPENDS zlibext
    )
    add_custom_target(tmp_zlib DEPENDS ${zlib_lib})

    add_library(zlib STATIC IMPORTED)
    set(ZLIB_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/zlibext/install/include")
    set_target_properties(
        zlib PROPERTIES
        IMPORTED_LOCATION "${zlib_lib}"
        IMPORTED_LOCATION_DEBUG "${zlib_lib_debug}"
        INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}"
    )

    add_dependencies(zlib tmp_zlib)

    if(FMILIB_INSTALL_SUBLIBS)
        install(FILES "${zlib_lib}" DESTINATION lib)
    endif()

endif()
