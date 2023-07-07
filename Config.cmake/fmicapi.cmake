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

if(NOT FMICAPIDIR)
set(FMICAPIDIR ${FMILIBRARYHOME}/src/CAPI)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMICAPIDIR}/include\"")

include(jmutil)

set(FMICAPI_LIBRARIES fmicapi)

set(FMICAPISOURCE
    src/FMI1/fmi1_capi_cs.c
    src/FMI1/fmi1_capi_me.c
    src/FMI1/fmi1_capi.c
    src/FMI2/fmi2_capi_cs.c
    src/FMI2/fmi2_capi_me.c
    src/FMI2/fmi2_capi.c
    src/FMI3/fmi3_capi_se.c
    src/FMI3/fmi3_capi_cs.c
    src/FMI3/fmi3_capi_me.c
    src/FMI3/fmi3_capi.c
)
set(FMICAPIHEADERS
    include/FMI1/fmi1_capi.h
    src/FMI1/fmi1_capi_impl.h
    include/FMI2/fmi2_capi.h
    src/FMI2/fmi2_capi_impl.h
    include/FMI3/fmi3_capi.h
    src/FMI3/fmi3_capi_impl.h
)

PREFIXLIST(FMICAPISOURCE  ${FMICAPIDIR}/)
PREFIXLIST(FMICAPIHEADERS ${FMICAPIDIR}/)

add_library(fmicapi ${FMILIBKIND} ${FMICAPISOURCE} ${FMICAPIHEADERS})
target_link_libraries(fmicapi PUBLIC ${JMUTIL_LIBRARIES})
target_include_directories(fmicapi
    PRIVATE
        ${FMICAPIDIR}/src
        ${FMILIB_FMI_STANDARD_HEADERS}
    PUBLIC
        ${FMILIB_CONFIG_INCLUDE_DIR}
        ${FMICAPIDIR}/include
)

endif(NOT FMICAPIDIR)
