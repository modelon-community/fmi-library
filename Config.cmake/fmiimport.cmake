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


################################################################################
# TODO: just copied this from XML for now, should be possible to create a macro
#   or function instead

### Generate FMIL source code ###

set(TEMPLATE_FILES
    fmi3_import_variable_generics
    fmi3_import_type_generics
    #fmi3_import_type_generics_h
)

set(TMPL_SRC_DST_LIST "")   # list of tuples (src, dst) that is passed to docker
set(TMPL_SRC_LIST "")       # list of template source files
set(TMPL_DST_LIST "")       # list of path to files generated from templates

# On Windows: create docker container that mounts cmake binary and source dir, and then
#   performs the preprocessing.
# On Linux: perform same as windows but without docker
if (WIN32)
    set(SRC_PREFIX /mnt_src)
    set(DST_PREFIX /mnt_bin)
else()
    set(SRC_PREFIX ${CMAKE_SOURCE_DIR})
    set(DST_PREFIX ${CMAKE_BINARY_DIR})
endif()

foreach(file ${TEMPLATE_FILES})
    # set temp loop variables
    set(SRC src/Import/templates/FMI3/${file}.c)
    set(DST src/Import/gen/FMI3/${file}.c)

    # create list of src/dst files as tuples:
    list(APPEND TMPL_SRC_DST_LIST \"${SRC_PREFIX}/${SRC}\" \"${DST_PREFIX}/${DST}\") # used as args, so need quoting

    # save paths (on windows, not docker) so we can set build dependencies:
    list(APPEND TMPL_SRC_LIST ${CMAKE_SOURCE_DIR}/${SRC})
    list(APPEND TMPL_DST_LIST ${CMAKE_BINARY_DIR}/${DST})
endforeach()
string (REPLACE ";" " " TMPL_SRC_DST_LIST_STR "${TMPL_SRC_DST_LIST}") # used as commandline arg, so sub-args need space separatation

add_custom_target(
    generate_functions_import ALL
    DEPENDS ${TMPL_DST_LIST} # these files exist in the binary dir
)

# specify the (platform-dependent) command for generating the DST files
set(CODEGEN_DIR ${SRC_PREFIX}/build/preprocess)
if (WIN32)
    # run via docker
    set(DOCKER_TAG_CODEGEN fmil_cmake_codegen)
    set(CMD_CODEGEN docker build -t ${DOCKER_TAG_CODEGEN} . && docker run -v "${CMAKE_SOURCE_DIR}:${SRC_PREFIX}" -v "${CMAKE_BINARY_DIR}:${DST_PREFIX}" ${DOCKER_TAG_CODEGEN} //bin/bash -c "chmod a+x ${CODEGEN_DIR}/preprocess*.sh && ${CODEGEN_DIR}/preprocess_list.sh ${TMPL_SRC_DST_LIST_STR}")

else() # linux
    # - run directly on command line
    # - command is wrapped in bash -c "..." because otherwise the (*) will be taken verbatim
    set(CMD_CODEGEN bash -c "chmod a+x ${CODEGEN_DIR}/preprocess*.sh && ${CODEGEN_DIR}/preprocess_list.sh ${TMPL_SRC_DST_LIST_STR}")
endif()

add_custom_command(
    OUTPUT ${TMPL_DST_LIST}
    DEPENDS ${TMPL_SRC_LIST}
    COMMAND ${CMD_CODEGEN}
    COMMAND_EXPAND_LISTS
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/preprocess
    COMMENT "generating C code from macro templates... (command: ${CMD_CODEGEN})"
)

################################################################################

set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIIMPORTDIR}/include\"")

include_directories(
    "${FMIIMPORTDIR}"
    "${FMIIMPORTDIR}/include"
    "${FMILIB_THIRDPARTYLIBS}/FMI/"
    "${CMAKE_BINARY_DIR}/src/Import"
)
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

	include/FMI3/fmi3_import.h
	include/FMI3/fmi3_import_capi.h
	include/FMI3/fmi3_import_type.h
	include/FMI3/fmi3_import_unit.h
	include/FMI3/fmi3_import_variable.h
	include/FMI3/fmi3_import_variable_list.h
	include/FMI3/fmi3_import_dimension.h
	include/FMI3/fmi3_import_dimension_list.h
	include/FMI3/fmi3_import_convenience.h

	include/FMI/fmi_import_context.h
	include/FMI/fmi_import_util.h
 )
							
set(FMIIMPORT_PRIVHEADERS
	src/FMI1/fmi1_import_impl.h
	src/FMI1/fmi1_import_variable_list_impl.h

	src/FMI2/fmi2_import_impl.h
	src/FMI2/fmi2_import_variable_list_impl.h

	src/FMI3/fmi3_import_impl.h
	src/FMI3/fmi3_import_variable_list_impl.h
	src/FMI3/fmi3_import_dimension_list_impl.h
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

	src/FMI3/fmi3_import_capi.c
	src/FMI3/fmi3_import_type.c
	src/FMI3/fmi3_import_unit.c
	src/FMI3/fmi3_import_variable.c
	src/FMI3/fmi3_import_variable_list.c
	src/FMI3/fmi3_import_dimension.c
	src/FMI3/fmi3_import_dimension_list.c
	src/FMI3/fmi3_import.c
	src/FMI3/fmi3_import_convenience.c
	)

PREFIXLIST(FMIIMPORTSOURCE  ${FMIIMPORTDIR}/)

add_library(fmiimport ${FMILIBKIND} ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS})
target_link_libraries(fmiimport ${JMUTIL_LIBRARIES} ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES})
add_dependencies(fmiimport generate_functions_import)
#target_link_libraries(fmiimportshared fmiimport)

#add_library(fmiimport_shared SHARED ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS} )
#target_link_libraries(fmiimport_shared fmiimport ${JMUTIL_LIBRARIES} ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES})
#install(TARGETS fmiimport_shared
#	ARCHIVE DESTINATION lib
#	LIBRARY DESTINATION lib
#	RUNTIME DESTINATION lib
#)
endif(NOT FMIIMPORTDIR)
