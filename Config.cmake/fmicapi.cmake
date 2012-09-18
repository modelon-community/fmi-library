#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT FMICAPIDIR)
set(FMICAPIDIR ${FMILIBRARYHOME}/src/CAPI)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMICAPIDIR}/include\"")

include(jmutil)

include_directories("${FMICAPIDIR}/include" "${FMICAPIDIR}/src")
set(FMICAPI_LIBRARIES fmicapi)

set(FMICAPISOURCE
    src/FMI1/fmi1_capi_cs.c
    src/FMI1/fmi1_capi_me.c
    src/FMI1/fmi1_capi.c
    src/FMI2/fmi2_capi_cs.c
    src/FMI2/fmi2_capi_me.c
    src/FMI2/fmi2_capi.c
)
set(FMICAPIHEADERS
	include/FMI1/fmi1_capi.h	
	src/FMI1/fmi1_capi_impl.h
	include/FMI2/fmi2_capi.h	
	src/FMI2/fmi2_capi_impl.h
)
 
include_directories(${FMILIB_FMI_STANDARD_HEADERS})

PREFIXLIST(FMICAPISOURCE  ${FMICAPIDIR}/)
PREFIXLIST(FMICAPIHEADERS ${FMICAPIDIR}/)

add_library(fmicapi ${FMILIBKIND} ${FMICAPISOURCE} ${FMICAPIHEADERS})

target_link_libraries(fmicapi ${JMUTIL_LIBRARIES})

# install(DIRECTORY ${FMIXMLDIR}/include DESTINATION .)
# install(DIRECTORY ${FMICAPIDIR}/include DESTINATION .)
#install(DIRECTORY ${JMRUNTIMEHOME}/FMI/ZIP/include DESTINATION include)

endif(NOT FMICAPIDIR)
