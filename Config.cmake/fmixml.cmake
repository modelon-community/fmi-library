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

################################################################################
#generate c source from Bison and Flex files
#Download Flex 2.5.4 and Bison 2.7 for windows from http://sourceforge.net/projects/winflexbison
if (WIN32)
    set(FMILIB_BUILD_LEX_AND_PARSER_FILES FALSE CACHE BOOL "Generate scanner and parser for variable name syntax checking. Requiers Flex and Bison commands to be specified. Please read src/XML/NOTE" )
    if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
        set(BISON_COMMAND ${FMILIB_THIRDPARTYLIBS}/winflexbison/win_bison.exe CACHE PATH "Command for running bison, e.g C:/win_bison.exe")
        set(FLEX_COMMAND ${FMILIB_THIRDPARTYLIBS}/winflexbison/win_flex.exe CACHE PATH "Command for running flex, e.g C:/win_flex.exe")
    else()
        #Remove variables from cache -> GUI if not used
        unset(BISON_COMMAND CACHE)
        unset(FLEX_COMMAND CACHE)
    endif()
endif()

#Folder to put generated files in
set(FMIXMLGENDIR ${FMIXMLDIR}/src-gen)

#Build BISON files
set(USE_DEVELOPER_BUILD FALSE) #Enable/disable developer(debug) build
if (${USE_DEVELOPER_BUILD})
	set(BISON_FMIX_COMMAND_DEBUG -v -t)
	set(BISON_FMI2_OUT_DEBUG ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.output)
	set(BISON_FMI1_OUT_DEBUG ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.output)
endif()
set(BISON_FMI2_SRC ${FMIXMLDIR}/src/FMI2/fmi2_xml_variable_name_parser.y)
set(BISON_FMI1_SRC ${FMIXMLDIR}/src/FMI1/fmi1_xml_variable_name_parser.y)
set(BISON_FMI2_OUT_HEADERS ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.tab.h)
set(BISON_FMI1_OUT_HEADERS ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.tab.h)
set(BISON_FMI2_OUT_SRC ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.tab.c)
set(BISON_FMI1_OUT_SRC ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.tab.c)
set(BISON_FMI2_OUT ${BISON_FMI2_OUT_SRC} ${BISON_FMI2_OUT_HEADERS} ${BISON_FMI2_OUT_DEBUG})
set(BISON_FMI1_OUT ${BISON_FMI1_OUT_SRC} ${BISON_FMI1_OUT_HEADERS} ${BISON_FMI1_OUT_DEBUG})
if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
	add_custom_command(OUTPUT ${BISON_FMI2_OUT} COMMAND ${BISON_COMMAND} ${BISON_FMIX_COMMAND_DEBUG} --no-lines -Dapi.prefix=yyfmi2 -d ${BISON_FMI2_SRC} DEPENDS ${BISON_FMI2_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI2)
	add_custom_command(OUTPUT ${BISON_FMI1_OUT} COMMAND ${BISON_COMMAND} ${BISON_FMIX_COMMAND_DEBUG} --no-lines -Dapi.prefix=yyfmi1 -d ${BISON_FMI1_SRC} DEPENDS ${BISON_FMI1_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI1)
endif()

#Build FLEX files
set(FLEX_FMI2_SRC ${FMIXMLDIR}/src/FMI2/fmi2_xml_variable_name_scan.l)
set(FLEX_FMI1_SRC ${FMIXMLDIR}/src/FMI1/fmi1_xml_variable_name_scan.l)
set(FLEX_FMI2_OUT_HEADERS ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_lex.h)
set(FLEX_FMI1_OUT_HEADERS ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_lex.h)
set(FLEX_FMI2_OUT_SRC ${FMIXMLGENDIR}/FMI2/lex.yyfmi2.c)
set(FLEX_FMI1_OUT_SRC ${FMIXMLGENDIR}/FMI1/lex.yyfmi1.c)
set(FLEX_FMI2_OPT_ARG --noline --header-file=${FLEX_FMI2_OUT_HEADERS} -Pyyfmi2)
set(FLEX_FMI1_OPT_ARG --noline --header-file=${FLEX_FMI1_OUT_HEADERS} -Pyyfmi1)

if (CMAKE_HOST_WIN32)
	set(FLEX_FMI2_OPT_ARG ${FLEX_FMI2_OPT_ARG})
	set(FLEX_FMI1_OPT_ARG ${FLEX_FMI1_OPT_ARG})
endif()
if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
	add_custom_command(OUTPUT ${FLEX_FMI2_OUT_SRC} ${FLEX_FMI2_OUT_HEADERS} COMMAND ${FLEX_COMMAND} ${FLEX_FMI2_OPT_ARG} ${FLEX_FMI2_SRC} DEPENDS ${BISON_FMI2_OUT} ${FLEX_FMI2_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI2)
	add_custom_command(OUTPUT ${FLEX_FMI1_OUT_SRC} ${FLEX_FMI1_OUT_HEADERS} COMMAND ${FLEX_COMMAND} ${FLEX_FMI1_OPT_ARG} ${FLEX_FMI1_SRC} DEPENDS ${BISON_FMI1_OUT} ${FLEX_FMI1_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI1)
endif()

if(WIN32)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

#end of generate c source from Bison and Flex files
################################################################################

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

### Add the options of expat to the cache and add it as subdirectory.
set(BUILD_tools OFF CACHE BOOL "build the xmlwf tool for expat library")
set(BUILD_examples OFF CACHE BOOL "build the examples for expat library")
set(BUILD_tests OFF CACHE BOOL "build the tests for expat library")
set(BUILD_shared OFF CACHE BOOL "build a shared expat library")
add_subdirectory(ThirdParty/Expat/expat-2.1.0)

set(EXPAT_INCLUDE_DIRS ThirdParty/Expat/expat-2.1.0/lib)
include_directories("${EXPAT_INCLUDE_DIRS}" "${FMILIB_THIRDPARTYLIBS}/FMI/" "${FMIXMLGENDIR}/FMI1" "${FMIXMLGENDIR}/FMI2")

PREFIXLIST(FMIXMLSOURCE  ${FMIXMLDIR}/)
PREFIXLIST(FMIXMLHEADERS ${FMIXMLDIR}/)

list(APPEND FMIXMLSOURCE
    ${BISON_FMI2_OUT_SRC}
    ${BISON_FMI1_OUT_SRC}
    ${FLEX_FMI2_OUT_SRC}
    ${FLEX_FMI1_OUT_SRC}
)

debug_message(STATUS "adding fmixml")

add_library(fmixml ${FMILIBKIND} ${FMIXMLSOURCE} ${FMIXMLHEADERS})

target_link_libraries(fmixml ${JMUTIL_LIBRARIES} expat)

endif(NOT FMIXMLDIR)
