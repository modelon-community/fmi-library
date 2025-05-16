#    Copyright (C) 2012 Modelon AB
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

if(NOT FMIZIPDIR)
set(FMIZIPDIR ${CMAKE_SOURCE_DIR}/src/ZIP)

include(jmutil)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIZIPDIR}/include\"")

if(FMILIB_SYSTEM_MINIZIP)
    find_package(MiniZip REQUIRED)
    add_library(minizip UNKNOWN IMPORTED)
    target_link_libraries(minizip INTERFACE ${MINIZIP_LIBRARY})
    target_include_directories(minizip INTERFACE ${MINIZIP_INCLUDE_DIR})
    set_target_properties(minizip PROPERTIES IMPORTED_LOCATION ${MINIZIP_LIBRARY})
else()
    set(FMILIB_MINIZIP_SOURCE_DIR  ${ZLIB_SOURCE_DIR}/contrib/minizip)
    set(FMILIB_MINIZIP_INCLUDE_DIR ${ZLIB_SOURCE_DIR}/contrib/minizip)
    add_subdirectory(Config.cmake/Minizip)
endif()

set(FMIZIPSOURCE
    ${FMIZIPDIR}/src/fmi_zip_unzip.c
    ${FMIZIPDIR}/src/fmi_zip_zip.c
    ${FMIZIPDIR}/src/fmi_miniunz.c
    ${FMIZIPDIR}/src/fmi_minizip.c
)

set(FMIZIPHEADERS
    ${FMIZIPDIR}/include/FMI/fmi_zip_unzip.h
    ${FMIZIPDIR}/include/FMI/fmi_zip_zip.h
)

add_library(fmizip STATIC ${FMIZIPSOURCE} ${FMIZIPHEADERS})
target_link_libraries(fmizip
    PRIVATE minizip
    PUBLIC  jmutils
)
target_include_directories(fmizip
    PRIVATE
        ${ZLIB_INCLUDE_DIRS}
        ${FMILIB_THIRDPARTYLIBS}/FMI
    PUBLIC
        ${FMILIB_CONFIG_INCLUDE_DIR}
        ${FMIZIPDIR}/include
)

if(FMILIB_SYSTEM_MINIZIP AND TARGET zlib)
    target_link_libraries(fmizip PRIVATE zlib)
else()
    target_include_directories(fmizip PRIVATE ${FMILIB_MINIZIP_INCLUDE_DIR})
endif()

endif(NOT FMIZIPDIR)
