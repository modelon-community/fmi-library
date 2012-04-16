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
)

PREFIXLIST(FMIIMPORTSOURCE  ${FMIIMPORTDIR}/)

add_library(fmiimport ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS})

target_link_libraries(fmiimport ${JMUTIL_LIBRARIES} ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES})

install(TARGETS fmiimport jm_utils fmi_zip fmixml fmicapi
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)
#install(DIRECTORY ${FMIIMPORTDIR}/include)
#install(DIRECTORY ${FMIIMPORTDIR}/include DESTINATION include)
#install(DIRECTORY ${JMRUNTIMEHOME}/Util DESTINATION include)
#install(DIRECTORY ${THIRDPARTYLIBS}/FMI DESTINATION include)
#install(DIRECTORY ${FMIXMLDIR}/include DESTINATION include)
#install(DIRECTORY ${FMICAPIDIR}/include DESTINATION include)
#install(DIRECTORY ${JMRUNTIMEHOME}/FMI/ZIP/include DESTINATION include)




#/I "C:\jmodelica_testbuild2005\Expat"
#/I "C:\jmodelica.org\ThirdParty\Expat\expat-2.0.1\lib" 
#/I "C:\jmodelica.org\JMI\FMI\ZIP\include" /I "C:\jmodelica.org\JMI\FMI\ZIP\src" /I "C:\jmodelica.org\ThirdParty\Minizip\minizip" /I "C:\jmodelica.org\ThirdParty\Zlib\zlib-1.2.6" /I "C:\jmodelica_testbuild2005\zlib" /I "C:\jmodelica.org\JMI\FMI\Import" /I "C:\jmodelica.org\JMI\FMI\Import\include" /I "C:\jmodelica.org\JMI\Test" /D "WIN32" /D "_WINDOWS" /D "XML_STATIC" /D "FMI_XML_QUERY" /D "ZLIB_STATIC" /D "NDEBUG" /D "_CRT_SECURE_NO_WARNINGS" /D "CMAKE_INTDIR=\"Release\"" /D "_MBCS"

endif(NOT FMIIMPORTDIR)
