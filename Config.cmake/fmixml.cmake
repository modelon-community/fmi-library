#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 3 of the License.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

if(NOT FMIXMLDIR)
set(FMIXMLDIR ${FMILIBRARYHOME}/src/XML/)
include(jmutil)

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIXMLDIR}/include\"")

include_directories("${FMIXMLDIR}/include" "${FMILIB_THIRDPARTYLIBS}/FMI/")
set(FMIXML_LIBRARIES fmixml)

set(FMIXMLHEADERS
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
    # include/FMI1/fmi1_xml_variable_list.h
    # src/FMI1/fmi1_xml_variable_list_impl.h
    include/FMI1/fmi1_xml_capabilities.h
    src/FMI1/fmi1_xml_capabilities_impl.h
	# src/FMI1/fmi1_xml_query.h
	include/Common/fmi_xml_context.h
	src/Common/fmi_xml_context_impl.h
 )

set(FMIXMLSOURCE
    src/FMI1/fmi1_xml_parser.c
    src/FMI1/fmi1_xml_model_description.c
    src/FMI1/fmi1_xml_type.c
    src/FMI1/fmi1_xml_unit.c
    src/FMI1/fmi1_xml_vendor_annotations.c
    src/FMI1/fmi1_xml_variable.c
    # src/FMI1/fmi1_xml_variable_list.c
    src/FMI1/fmi1_xml_capabilities.c
    src/FMI1/fmi1_xml_cosim.c
	# src/FMI1/fmi1_xml_query.c
	src/Common/fmi_xml_context.c
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
		-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/ExpatInst
)

ExternalProject_Add(
	expatex
	PREFIX "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.0.1"
	SOURCE_DIR "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.0.1"
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
	COMMAND ${CMAKE_COMMAND} -E echo "Running:  ${CMAKE_COMMAND} ${EXPAT_SETTINGS} ${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.0.1"
	COMMAND ${CMAKE_COMMAND} ${EXPAT_SETTINGS} "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.0.1"
	DEPENDS ${CMAKE_BINARY_DIR}/CMakeCache.txt
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ExpatEx
)

add_dependencies(expatex ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  
set(expatlib "${CMAKE_BINARY_DIR}/ExpatEx/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}")
  
add_library(expat STATIC IMPORTED)

set_target_properties(
	expat PROPERTIES 
		IMPORTED_LOCATION "${expatlib}"
)

if(FMILIB_INSTALL_SUBLIBS)
	install(FILES "${expatlib}"	DESTINATION lib)
endif()

set(EXPAT_INCLUDE_DIRS "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.0.1/lib")
include_directories("${EXPAT_INCLUDE_DIRS}" "${FMILIB_THIRDPARTYLIBS}/FMI/")

PREFIXLIST(FMIXMLSOURCE  ${FMIXMLDIR}/)
PREFIXLIST(FMIXMLHEADERS ${FMIXMLDIR}/)

debug_message(STATUS "adding fmixml")

add_library(fmixml ${FMILIBKIND} ${FMIXMLSOURCE} ${FMIXMLHEADERS})

add_dependencies(fmixml expatex)

target_link_libraries(fmixml ${JMUTIL_LIBRARIES} expat)

endif(NOT FMIXMLDIR)
