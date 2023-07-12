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
set(FMIZIPDIR ${FMILIBRARYHOME}/src/ZIP)

include(jmutil)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIZIPDIR}/include\"")

add_subdirectory(Config.cmake/Minizip)

set(FMIZIPSOURCE
    ${FMIZIPDIR}/src/fmi_zip_unzip.c
    ${FMIZIPDIR}/src/fmi_zip_zip.c
)

set(FMIZIPHEADERS
    ${FMIZIPDIR}/include/FMI/fmi_zip_unzip.h
    ${FMIZIPDIR}/include/FMI/fmi_zip_zip.h
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DZLIB_STATIC")

add_library(fmizip STATIC ${FMIZIPSOURCE} ${FMIZIPHEADERS})
target_link_libraries(fmizip
    PRIVATE minizip
    PUBLIC  jmutils
)
target_include_directories(fmizip
    PRIVATE
        ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip
        ${FMILIB_THIRDPARTYLIBS}/FMI
        ${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.2.6
        ${FMILibrary_BINARY_DIR}/zlib
    PUBLIC
        ${FMILIB_CONFIG_INCLUDE_DIR}
        ${FMIZIPDIR}/include
)

endif(NOT FMIZIPDIR)
