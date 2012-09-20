#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT FMIIMPORTDIR)
set(FMIIMPORTDIR ${FMILIBRARYHOME}/src/Import)
include(jmutil)
include(fmixml)
include(fmicapi)

set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIIMPORTDIR}/include\"")

include_directories("${FMIIMPORTDIR}" "${FMIIMPORTDIR}/include" "${FMILIB_THIRDPARTYLIBS}/FMI/")
set(FMIIMPORT_LIBRARIES fmiimport)

set(FMIIMPORT_PUBHEADERS
	include/FMI1/fmi1_import.h
	include/FMI1/fmi1_import_capi.h
	include/FMI1/fmi1_import_capabilities.h
	include/FMI1/fmi1_import_cosim.h
	include/FMI1/fmi1_import_type.h
	include/FMI1/fmi1_import_unit.h
	include/FMI1/fmi1_import_variable.h
	include/FMI1/fmi1_import_variable_list.h
	include/FMI1/fmi1_import_vendor_annotations.h
	include/FMI1/fmi1_import_convenience.h

	include/FMI2/fmi2_import.h
	include/FMI2/fmi2_import_capi.h
	include/FMI2/fmi2_import_type.h
	include/FMI2/fmi2_import_unit.h
	include/FMI2/fmi2_import_variable.h
	include/FMI2/fmi2_import_variable_list.h
	include/FMI2/fmi2_import_convenience.h

	include/FMI/fmi_import_context.h
	include/FMI/fmi_import_util.h
 )
							
set(FMIIMPORT_PRIVHEADERS
	src/FMI1/fmi1_import_impl.h
	src/FMI1/fmi1_import_variable_list_impl.h

	src/FMI2/fmi2_import_impl.h
	src/FMI2/fmi2_import_variable_list_impl.h
 )
 
PREFIXLIST(FMIIMPORT_PRIVHEADERS ${FMIIMPORTDIR}/)
PREFIXLIST(FMIIMPORT_PUBHEADERS ${FMIIMPORTDIR}/)

set(FMIIMPORTHEADERS
	${FMIIMPORT_PUBHEADERS} ${FMIIMPORT_PRIVHEADERS}
 )

set(FMIIMPORTSOURCE
	src/FMI/fmi_import_context.c
	src/FMI/fmi_import_util.c
	
	src/FMI1/fmi1_import_cosim.c
	src/FMI1/fmi1_import_capi.c
	src/FMI1/fmi1_import_type.c
	src/FMI1/fmi1_import_unit.c
	src/FMI1/fmi1_import_variable.c
	src/FMI1/fmi1_import_variable_list.c
	src/FMI1/fmi1_import_vendor_annotations.c
	src/FMI1/fmi1_import.c
	src/FMI1/fmi1_import_capabilities.c
	src/FMI1/fmi1_import_convenience.c

	src/FMI2/fmi2_import_capi.c
	src/FMI2/fmi2_import_type.c
	src/FMI2/fmi2_import_unit.c
	src/FMI2/fmi2_import_variable.c
	src/FMI2/fmi2_import_variable_list.c
	src/FMI2/fmi2_import.c
	src/FMI2/fmi2_import_convenience.c
	)

PREFIXLIST(FMIIMPORTSOURCE  ${FMIIMPORTDIR}/)

add_library(fmiimport ${FMILIBKIND} ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS})
target_link_libraries(fmiimport ${JMUTIL_LIBRARIES} ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES})
#target_link_libraries(fmiimportshared fmiimport)

#add_library(fmiimport_shared SHARED ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS} )
#target_link_libraries(fmiimport_shared fmiimport ${JMUTIL_LIBRARIES} ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES})
#install(TARGETS fmiimport_shared
#	ARCHIVE DESTINATION lib
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION lib
#)
endif(NOT FMIIMPORTDIR)
