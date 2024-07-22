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

if(NOT FMIXMLDIR)
set(FMIXMLDIR ${CMAKE_SOURCE_DIR}/src/XML/)
include(jmutil)

################################################################################
#generate c source from Bison and Flex files
#Download Flex 2.5.4 and Bison 2.7 for windows from http://sourceforge.net/projects/winflexbison
if (WIN32)
    set(FMILIB_BUILD_LEX_AND_PARSER_FILES FALSE CACHE BOOL "Generate scanner and parser for variable name syntax checking. Requiers Flex and Bison commands to be specified. Please read src/XML/NOTE" )
    if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
        set(BISON_COMMAND ${FMILIB_THIRDPARTYLIBS}/winflexbison/win_bison.exe CACHE PATH "Command for running bison, e.g C:/win_bison.exe")
        set(FLEX_COMMAND ${FMILIB_THIRDPARTYLIBS}/winflexbison/win_flex.exe CACHE PATH "Command for running flex, e.g C:/win_flex.exe")

        # Verify that flex and bison exists - they are not included by default
        # in ThirdParty directory, and must be downloaded from some 3rd party.
        foreach(cmd BISON_COMMAND FLEX_COMMAND)
            if (NOT EXISTS ${${cmd}})
                message(FATAL_ERROR "Can't find program: ${${cmd}}")
            endif()
        endforeach()
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
    set(BISON_FMI3_OUT_DEBUG ${FMIXMLGENDIR}/FMI3/fmi3_xml_variable_name_parser.output)
    set(BISON_FMI2_OUT_DEBUG ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.output)
    set(BISON_FMI1_OUT_DEBUG ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.output)
endif()
set(BISON_FMI3_SRC ${FMIXMLDIR}/src/FMI3/fmi3_xml_variable_name_parser.y)
set(BISON_FMI2_SRC ${FMIXMLDIR}/src/FMI2/fmi2_xml_variable_name_parser.y)
set(BISON_FMI1_SRC ${FMIXMLDIR}/src/FMI1/fmi1_xml_variable_name_parser.y)
set(BISON_FMI3_OUT_HEADERS ${FMIXMLGENDIR}/FMI3/fmi3_xml_variable_name_parser.tab.h)
set(BISON_FMI2_OUT_HEADERS ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.tab.h)
set(BISON_FMI1_OUT_HEADERS ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.tab.h)
set(BISON_FMI3_OUT_SRC ${FMIXMLGENDIR}/FMI3/fmi3_xml_variable_name_parser.tab.c)
set(BISON_FMI2_OUT_SRC ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_parser.tab.c)
set(BISON_FMI1_OUT_SRC ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_parser.tab.c)
set(BISON_FMI3_OUT ${BISON_FMI3_OUT_SRC} ${BISON_FMI3_OUT_HEADERS} ${BISON_FMI3_OUT_DEBUG})
set(BISON_FMI2_OUT ${BISON_FMI2_OUT_SRC} ${BISON_FMI2_OUT_HEADERS} ${BISON_FMI2_OUT_DEBUG})
set(BISON_FMI1_OUT ${BISON_FMI1_OUT_SRC} ${BISON_FMI1_OUT_HEADERS} ${BISON_FMI1_OUT_DEBUG})
if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
    add_custom_command(OUTPUT ${BISON_FMI3_OUT} COMMAND ${BISON_COMMAND} ${BISON_FMIX_COMMAND_DEBUG} --no-lines -Dapi.prefix=yyfmi3 -d ${BISON_FMI3_SRC} DEPENDS ${BISON_FMI3_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI3)
    add_custom_command(OUTPUT ${BISON_FMI2_OUT} COMMAND ${BISON_COMMAND} ${BISON_FMIX_COMMAND_DEBUG} --no-lines -Dapi.prefix=yyfmi2 -d ${BISON_FMI2_SRC} DEPENDS ${BISON_FMI2_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI2)
    add_custom_command(OUTPUT ${BISON_FMI1_OUT} COMMAND ${BISON_COMMAND} ${BISON_FMIX_COMMAND_DEBUG} --no-lines -Dapi.prefix=yyfmi1 -d ${BISON_FMI1_SRC} DEPENDS ${BISON_FMI1_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI1)
endif()

#Build FLEX files
set(FLEX_FMI3_SRC ${FMIXMLDIR}/src/FMI3/fmi3_xml_variable_name_scan.l)
set(FLEX_FMI2_SRC ${FMIXMLDIR}/src/FMI2/fmi2_xml_variable_name_scan.l)
set(FLEX_FMI1_SRC ${FMIXMLDIR}/src/FMI1/fmi1_xml_variable_name_scan.l)
set(FLEX_FMI3_OUT_HEADERS ${FMIXMLGENDIR}/FMI3/fmi3_xml_variable_name_lex.h)
set(FLEX_FMI2_OUT_HEADERS ${FMIXMLGENDIR}/FMI2/fmi2_xml_variable_name_lex.h)
set(FLEX_FMI1_OUT_HEADERS ${FMIXMLGENDIR}/FMI1/fmi1_xml_variable_name_lex.h)
set(FLEX_FMI3_OUT_SRC ${FMIXMLGENDIR}/FMI3/lex.yyfmi3.c)
set(FLEX_FMI2_OUT_SRC ${FMIXMLGENDIR}/FMI2/lex.yyfmi2.c)
set(FLEX_FMI1_OUT_SRC ${FMIXMLGENDIR}/FMI1/lex.yyfmi1.c)
set(FLEX_FMI3_OPT_ARG --noline --header-file=${FLEX_FMI3_OUT_HEADERS} -Pyyfmi3)
set(FLEX_FMI2_OPT_ARG --noline --header-file=${FLEX_FMI2_OUT_HEADERS} -Pyyfmi2)
set(FLEX_FMI1_OPT_ARG --noline --header-file=${FLEX_FMI1_OUT_HEADERS} -Pyyfmi1)

if (CMAKE_HOST_WIN32)
    set(FLEX_FMI3_OPT_ARG ${FLEX_FMI3_OPT_ARG})
    set(FLEX_FMI2_OPT_ARG ${FLEX_FMI2_OPT_ARG})
    set(FLEX_FMI1_OPT_ARG ${FLEX_FMI1_OPT_ARG})
endif()
if (${FMILIB_BUILD_LEX_AND_PARSER_FILES})
    add_custom_command(OUTPUT ${FLEX_FMI3_OUT_SRC} ${FLEX_FMI3_OUT_HEADERS} COMMAND ${FLEX_COMMAND} ${FLEX_FMI3_OPT_ARG} ${FLEX_FMI3_SRC} DEPENDS ${BISON_FMI3_OUT} ${FLEX_FMI3_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI3)
    add_custom_command(OUTPUT ${FLEX_FMI2_OUT_SRC} ${FLEX_FMI2_OUT_HEADERS} COMMAND ${FLEX_COMMAND} ${FLEX_FMI2_OPT_ARG} ${FLEX_FMI2_SRC} DEPENDS ${BISON_FMI2_OUT} ${FLEX_FMI2_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI2)
    add_custom_command(OUTPUT ${FLEX_FMI1_OUT_SRC} ${FLEX_FMI1_OUT_HEADERS} COMMAND ${FLEX_COMMAND} ${FLEX_FMI1_OPT_ARG} ${FLEX_FMI1_SRC} DEPENDS ${BISON_FMI1_OUT} ${FLEX_FMI1_SRC} WORKING_DIRECTORY ${FMIXMLGENDIR}/FMI1)
endif()

if(WIN32)
    add_definitions(-DYY_NO_UNISTD_H)
endif()

#end of generate c source from Bison and Flex files
################################################################################

# set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIXMLDIR}/include\"")

set(FMIXML_EXPAT_DIR "${FMILIB_THIRDPARTYLIBS}/Expat/expat-2.5.0")

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

    include/FMI3/fmi3_xml_model_description.h
    src/FMI3/fmi3_xml_model_description_impl.h
    include/FMI3/fmi3_xml_model_structure.h
    src/FMI3/fmi3_xml_model_structure_impl.h
    include/FMI3/fmi3_xml_terminals_and_icons.h
    src/FMI3/fmi3_xml_terminals_and_icons_impl.h
    src/FMI3/fmi3_xml_model_description_parser.h
    src/FMI3/fmi3_xml_terminals_and_icons_parser.h
    src/FMI3/fmi3_xml_parser_util.h
    src/FMI3/fmi3_xml_parser.h
    include/FMI3/fmi3_xml_type.h
    src/FMI3/fmi3_xml_type_impl.h
    include/FMI3/fmi3_xml_unit.h
    src/FMI3/fmi3_xml_unit_impl.h
    include/FMI3/fmi3_xml_variable.h
    src/FMI3/fmi3_xml_variable_impl.h
    src/FMI3/fmi3_xml_parser_lists.h
    src/FMI3/fmi3_xml_parser_context_impl.h
    src/
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

    src/FMI3/fmi3_xml_parser.c
    src/FMI3/fmi3_xml_parser_util.c
    src/FMI3/fmi3_xml_model_description.c
    src/FMI3/fmi3_xml_model_structure.c
    src/FMI3/fmi3_xml_type.c
    src/FMI3/fmi3_xml_unit.c
    src/FMI3/fmi3_xml_vendor_annotations.c
    src/FMI3/fmi3_xml_variable.c
    src/FMI3/fmi3_xml_terminals_and_icons.c
)

include(ExternalProject)

# The *_POSTFIX variables are set because it makes it easier to determine the name of
# the lib expat will produce at configure time. Note that Expat has some special handling
# for it for MSVC which this in effect negates. https://github.com/libexpat/libexpat/pull/316
set(EXPAT_SETTINGS
    -DEXPAT_BUILD_TOOLS:BOOLEAN=OFF
    -DEXPAT_BUILD_EXAMPLES:BOOLEAN=OFF
    -DEXPAT_BUILD_TESTS:BOOLEAN=OFF
    -DEXPAT_SHARED_LIBS:BOOLEAN=OFF
    -DEXPAT_DTD:BOOLEAN=OFF
    -DEXPAT_NS:BOOLEAN=OFF
    -DEXPAT_MSVC_STATIC_CRT:BOOLEAN=${FMILIB_BUILD_WITH_STATIC_RTLIB}
    -DEXPAT_DEBUG_POSTFIX:STRING=
    -DEXPAT_RELEASE_POSTFIX:STRING=
    -DEXPAT_MINSIZEREL_POSTFIX:STRING=
    -DEXPAT_RELWITHDEBINFO_POSTFIX:STRING=
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOLEAN=ON
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
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
    COMMAND ${CMAKE_COMMAND} -E echo "Running:  ${CMAKE_COMMAND} -G \"${CMAKE_GENERATOR}\"  ${EXPAT_SETTINGS} ${FMIXML_EXPAT_DIR}"
    COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${EXPAT_SETTINGS} "${FMIXML_EXPAT_DIR}"
    DEPENDS ${CMAKE_BINARY_DIR}/CMakeCache.txt
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ExpatEx
)

# XXX: Maybe we could use FetchContent to find targets of expat? Then we hopefully
# wouldn't need below workarounds for guessing expatlib's location and name.
# Requires CMake 3.16 though.

if(MSVC)
    # Expat uses special naming with MSVC, which is mirrored here.
    set(EXPAT_LIB_PREFIX lib)
else()
    set(EXPAT_LIB_PREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
endif()

if("${CMAKE_CFG_INTDIR}" STREQUAL ".")
    # Ninja complains about 'ExpatEx/./libexpat.a' otherwise. Probably because
    # generator expressions in mergestaticlibs give slighlty different paths.
    set(expatlib "${CMAKE_BINARY_DIR}/ExpatEx/${EXPAT_LIB_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(expatlib "${CMAKE_BINARY_DIR}/ExpatEx/${CMAKE_CFG_INTDIR}/${EXPAT_LIB_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# Workaround to make it explicit that target 'expatex' produces 'expatlib'. (Ninja complains otherwise.)
add_custom_command(
    OUTPUT "${expatlib}"
    DEPENDS expatex
)
add_custom_target(tmp_expatlib DEPENDS ${expatlib})

add_library(expat STATIC IMPORTED)
set_target_properties(
    expat PROPERTIES
        IMPORTED_LOCATION "${expatlib}"
)
add_dependencies(expat tmp_expatlib)

if(FMILIB_INSTALL_SUBLIBS)
    install(FILES "${expatlib}" DESTINATION lib)
endif()

set(EXPAT_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ExpatEx/install/include)

PREFIXLIST(FMIXMLSOURCE  ${FMIXMLDIR}/)
PREFIXLIST(FMIXMLHEADERS ${FMIXMLDIR}/)

list(APPEND FMIXMLSOURCE
    ${BISON_FMI3_OUT_SRC}
    ${BISON_FMI2_OUT_SRC}
    ${BISON_FMI1_OUT_SRC}
    ${FLEX_FMI3_OUT_SRC}
    ${FLEX_FMI2_OUT_SRC}
    ${FLEX_FMI1_OUT_SRC}
)

add_library(fmixml STATIC ${FMIXMLSOURCE} ${FMIXMLHEADERS})
if(MSVC)
    target_compile_definitions(fmixml PUBLIC XML_STATIC)
endif()
target_link_libraries(fmixml
    PRIVATE expat
    PUBLIC  jmutils
)
target_include_directories(fmixml
    PRIVATE
        ${EXPAT_INCLUDE_DIRS}
        ${FMILIB_THIRDPARTYLIBS}/FMI/
        ${FMIXMLGENDIR}/FMI1
        ${FMIXMLGENDIR}/FMI2
        ${FMIXMLGENDIR}/FMI3
        ${CMAKE_BINARY_DIR}/src/XML
    PUBLIC
        ${FMILIB_CONFIG_INCLUDE_DIR}
        ${FMIXMLDIR}/include
)

endif(NOT FMIXMLDIR)
