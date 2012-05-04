if(NOT FMIIMPORTDIR)
set(FMIIMPORTDIR ${FMILIBRARYHOME}/src/Import)
include(jmutil)
include(fmixml)
include(fmicapi)

set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIIMPORTDIR}/include\"")

include_directories("${FMIIMPORTDIR}" "${FMIIMPORTDIR}/include" "${THIRDPARTYLIBS}/FMI/")
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
	include/Common/fmi_import_context.h
	include/Common/fmi_import_util.h
 )
							
set(FMIIMPORT_PRIVHEADERS
	src/FMI1/fmi1_import_impl.h
	src/FMI1/fmi1_import_variable_list_impl.h
 )
 
PREFIXLIST(FMIIMPORT_PRIVHEADERS ${FMIIMPORTDIR}/)
PREFIXLIST(FMIIMPORT_PUBHEADERS ${FMIIMPORTDIR}/)

set(FMIIMPORTHEADERS
	${FMIIMPORT_PUBHEADERS} ${FMIIMPORT_PRIVHEADERS}
 )

set(FMIIMPORTSOURCE
	src/Common/fmi_import_context.c
	src/FMI1/fmi1_import_cosim.c
	src/FMI1/fmi1_import_capi.c
	#src/FMI1/fmi1_import_query.c
	src/FMI1/fmi1_import_type.c
	src/FMI1/fmi1_import_unit.c
	src/FMI1/fmi1_import_variable.c
	src/FMI1/fmi1_import_variable_list.c
	src/FMI1/fmi1_import_vendor_annotations.c
	src/FMI1/fmi1_import.c
	#src/Common/fmi_import_util.h
	src/Common/fmi_import_util.c
	src/FMI1/fmi1_import_capabilities.c
	src/FMI1/fmi1_import_convenience.c
)

PREFIXLIST(FMIIMPORTSOURCE  ${FMIIMPORTDIR}/)

add_library(fmiimport ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS})
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
