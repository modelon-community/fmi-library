#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT FMIXMLDIR)
set(FMIXMLDIR ${FMILIBRARYHOME}/src/XML/)
include(jmutil)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIXMLDIR}/include\"")

include_directories("${FMIXMLDIR}/include" "${FMILIB_THIRDPARTYLIBS}/FMI/")
set(FMIXML_LIBRARIES fmixml)
set(FMIXML_EXPAT_DIR "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.1.0") 

set(FMIXMLHEADERS
	include/FMI/fmi_xml_context.h
	src/FMI/fmi_xml_context_impl.h

    include/FMI1/fmi1_xml_model_description.h
    src/FMI1/fmi1_xml_model_description_impl.h
    src/FMI1/fmi1_xml_parser.h
    include/FMI1/fmi1_xml_type.h
    src/FMI1/fmi1_xml_type_impl.h
    include/FMI1/fmi1_xml_unit.h
    src/FMI1/fmi1_xml_unit_impl.h
    include/FMI1/fmi1_xml_vendor_annotations.h
    src/FMI1/fmi1_xml_vendor_annotations_impl.h
    include/FMI1/fmi1_xml_variable.h
    src/FMI1/fmi1_xml_variable_impl.h
    include/FMI1/fmi1_xml_capabilities.h
    src/FMI1/fmi1_xml_capabilities_impl.h

    include/FMI2/fmi2_xml_model_description.h
    src/FMI2/fmi2_xml_model_description_impl.h
    include/FMI2/fmi2_xml_model_structure.h
    src/FMI2/fmi2_xml_model_structure_impl.h
    src/FMI2/fmi2_xml_parser.h
    include/FMI2/fmi2_xml_type.h
    src/FMI2/fmi2_xml_type_impl.h
    include/FMI2/fmi2_xml_unit.h
    src/FMI2/fmi2_xml_unit_impl.h
    include/FMI2/fmi2_xml_variable.h
    src/FMI2/fmi2_xml_variable_impl.h
 )

set(FMIXMLSOURCE
	src/FMI/fmi_xml_context.c

    src/FMI1/fmi1_xml_parser.c
    src/FMI1/fmi1_xml_model_description.c
    src/FMI1/fmi1_xml_type.c
    src/FMI1/fmi1_xml_unit.c
    src/FMI1/fmi1_xml_vendor_annotations.c
    src/FMI1/fmi1_xml_variable.c
    src/FMI1/fmi1_xml_capabilities.c
    src/FMI1/fmi1_xml_cosim.c

    src/FMI2/fmi2_xml_parser.c
    src/FMI2/fmi2_xml_model_description.c
    src/FMI2/fmi2_xml_model_structure.c
    src/FMI2/fmi2_xml_type.c
    src/FMI2/fmi2_xml_unit.c
	src/FMI2/fmi2_xml_vendor_annotations.c
	src/FMI2/fmi2_xml_variable.c
)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DXML_STATIC -DFMI_XML_QUERY")

include(ExternalProject)

set(EXPAT_SETTINGS
		-DBUILD_tools:BOOLEAN=OFF
		-DBUILD_examples:BOOLEAN=OFF
		-DBUILD_tests:BOOLEAN=OFF
		-DBUILD_shared:BOOLEAN=OFF
		-DXML_DTD:BOOLEAN=OFF
		-DXML_NS:BOOLEAN=OFF
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
		-DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
		-DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
		-DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
		-DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}
		-DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
		-DCMAKE_LINK_LIBRARY_FLAG:STRING=${CMAKE_LINK_LIBRARY_FLAG}
		-DCMAKE_MODULE_LINKER_FLAGS:STRING=${CMAKE_MODULE_LINKER_FLAGS}
		-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}	
		-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/ExpatEx/install
)

ExternalProject_Add(
	expatex
	PREFIX "${FMIXML_EXPAT_DIR}"
	SOURCE_DIR "${FMIXML_EXPAT_DIR}"
	CMAKE_CACHE_ARGS ${EXPAT_SETTINGS}
	BINARY_DIR ${CMAKE_BINARY_DIR}/ExpatEx
	INSTALL_DIR ${CMAKE_BINARY_DIR}/ExpatEx/install
	TMP_DIR     ${CMAKE_BINARY_DIR}/ExpatEx/tmp
    STAMP_DIR   ${CMAKE_BINARY_DIR}/ExpatEx/stamp
)

ExternalProject_Add_Step(
	expatex dependent_reconfigure
	DEPENDEES configure
	DEPENDERS build
	COMMAND ${CMAKE_COMMAND} -E echo "Running:  ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"  ${EXPAT_SETTINGS} ${FMIXML_EXPAT_DIR}"
	COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${EXPAT_SETTINGS} "${FMIXML_EXPAT_DIR}"
	DEPENDS ${CMAKE_BINARY_DIR}/CMakeCache.txt
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ExpatEx
)

add_dependencies(expatex ${CMAKE_BINARY_DIR}/CMakeCache.txt ${FMILIBRARYHOME}/CMakeLists.txt)
  
set(expatlib "${CMAKE_BINARY_DIR}/ExpatEx/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}")
  
add_library(expat STATIC IMPORTED)

set_target_properties(
	expat PROPERTIES 
		IMPORTED_LOCATION "${expatlib}"
)

add_dependencies(expat expatex)

if(FMILIB_INSTALL_SUBLIBS)
	install(FILES 
	"${CMAKE_BINARY_DIR}/ExpatEx/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}"
	DESTINATION lib)
endif()

set(EXPAT_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ExpatEx/install/include)

include_directories("${EXPAT_INCLUDE_DIRS}" "${FMILIB_THIRDPARTYLIBS}/FMI/")

PREFIXLIST(FMIXMLSOURCE  ${FMIXMLDIR}/)
PREFIXLIST(FMIXMLHEADERS ${FMIXMLDIR}/)

debug_message(STATUS "adding fmixml")

add_library(fmixml ${FMILIBKIND} ${FMIXMLSOURCE} ${FMIXMLHEADERS})

target_link_libraries(fmixml ${JMUTIL_LIBRARIES} expat)

endif(NOT FMIXMLDIR)
