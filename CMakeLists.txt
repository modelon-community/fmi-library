#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
	
# NOTE: CMake 2.8.6 is required since this is the version used in development.
# The script is KNOWN NOT TO WORK WITH 2.8.3 and below (ExternalProject 
# interface changes). CMake 2.8.4 and 2.8.5 are not tested.
cmake_minimum_required (VERSION 2.8.6 FATAL_ERROR)

# Prohibit in-source build
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source build is not supported. Please, use an empty directory for building the project.")
ENDIF()
project (FMILibrary)
set_property(GLOBAL PROPERTY PROPERTYPREDEFINED_TARGETS_FOLDER Global)
set(FMILIBRARYHOME ${FMILibrary_SOURCE_DIR})
set(FMILIBRARYBUILD ${FMILibrary_BINARY_DIR})

# User configuration options and parameters
SET(FMILIB_INSTALL_PREFIX ${FMILibrary_BINARY_DIR}/../install CACHE PATH "Prefix prepended to install directories")
set(FMILIB_THIRDPARTYLIBS  ${FMILibrary_SOURCE_DIR}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )
set(FMILIB_FMI_STANDARD_HEADERS  ${FMILIB_THIRDPARTYLIBS}/FMI/default CACHE PATH "Path to the FMI standard headers dir" )

option (FMILIB_DEFAULT_BUILD_TYPE_RELEASE "Default build-type used for Makefile generation is 'Release'. Can be overwritten by CMAKE_BUILD_TYPE command line switch." ON)

if(MSVC)
	option (FMILIB_BUILD_WITH_STATIC_RTLIB "Use static run-time libraries (/MT or /MTd linker flags)" OFF)
endif()

option(FMILIB_BUILD_STATIC_LIB	 "Build the library as static." ON)
option(FMILIB_BUILD_SHARED_LIB	 "Build the library as shared (dll/so/dylib)." ON)
option(FMILIB_INSTALL_SUBLIBS	 "Install sub-libraries (fmicapi. fmixml, jmutil, fmiimport, fmizip)" OFF)


IF(NOT (WIN32 OR CYGWIN OR APPLE))
# On LINUX position independent code (-fPIC) must be used on all files to be linked into .so
# On other systems this is not needed (either is default or relocation is done)
	if(NOT FMILIB_BUILD_SHARED_LIB)
		option(FMILIB_BUILD_FOR_SHARED_LIBS  "The static library 'fmilib' can be linked into shared libraries. Mostly needed on Linux." ON)
	else()
		set(FMILIB_BUILD_FOR_SHARED_LIBS ON CACHE INTERNAL "The static library 'fmilib' can be linked into shared libraries. Mostly needed on Linux." FORCE)
	endif()
else()
	set(FMILIB_BUILD_FOR_SHARED_LIBS OFF CACHE INTERNAL "The static library 'fmilib' can be linked into shared libraries. Mostly needed on Linux." FORCE)
endif()

option (FMILIB_GENERATE_DOXYGEN_DOC "Generate doxygen doc target" ON)

option (FMILIB_BUILD_TESTS "Build tests" ON)
option (FMILIB_BUILD_BEFORE_TESTS "Force build before testing" ON)
option(FMILIB_LINK_TEST_TO_SHAREDLIB "Link the tests to fmilib_shared (if built) instead of fmilib" ON)

option(FMILIB_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
option(FMILIB_ENABLE_LOG_LEVEL_DEBUG "Enable log level 'debug'. If the option is of then the debug level is not compiled in." OFF)
option(FMILIB_PRINT_DEBUG_MESSAGES "Enable printing of status messages from the build script. Intended for debugging." OFF)
mark_as_advanced(FMILIB_PRINT_DEBUG_MESSAGES FMILIB_DEBUG_TRACE)

if(NOT FMILIB_BUILD_SHARED_LIB AND NOT FMILIB_BUILD_STATIC_LIB)
	message(FATAL_ERROR "Either shared or static library build must be chosen")
endif()

if(FMILIB_BUILD_STATIC_LIB AND NOT FMILIB_BUILD_SHARED_LIB)
	set(FMILIB_STATIC_LIB_ONLY ON)
else()
	set(FMILIB_STATIC_LIB_ONLY OFF)
endif()

if(FMILIB_DEFAULT_BUILD_TYPE_RELEASE)
	set (FMILIB_DEFAULT_BUILD_TYPE "Release")
else()
	set (FMILIB_DEFAULT_BUILD_TYPE "Debug")
endif()

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE ${FMILIB_DEFAULT_BUILD_TYPE})
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_INSTALL_PREFIX ${FMILIB_INSTALL_PREFIX} CACHE INTERNAL "Prefix prepended to install directories" FORCE)

# debug_message is used to trace the build script
function(debug_message)
	if(FMILIB_PRINT_DEBUG_MESSAGES)
		message(STATUS "${ARGV}")
	endif()
endfunction()

IF(NOT CMAKE_CFG_INTDIR)
 SET(CMAKE_CFG_INTDIR ${FMILIB_DEFAULT_BUILD_TYPE})
ENDIF(NOT CMAKE_CFG_INTDIR)

if(MSVC)
	# With C89 we're forced to use non-secure functions
	ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
	
	# Set the run-time library flag as controlled by the FMILIB_BUILD_WITH_STATIC_RTLIB option
	if(FMILIB_BUILD_WITH_STATIC_RTLIB)
		set(BUILD_WITH_STATIC_CRT YES)
		set(replace_from "/MD")
		set(replace_to "/MT")
	else()
		set(BUILD_WITH_STATIC_CRT NO)
		set(replace_from "/MT")
		set(replace_to "/MD")
	endif(FMILIB_BUILD_WITH_STATIC_RTLIB)

	foreach(flag_var
		CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
		CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
		if(${${flag_var}} MATCHES "${replace_from}")
			string(REPLACE "${replace_from}" "${replace_to}" tmp "${${flag_var}}")
			set(${flag_var} ${tmp} CACHE STRING "compiler flags" FORCE)
		endif()
	endforeach(flag_var)	
endif(MSVC)

IF(CMAKE_COMPILER_IS_GNUCC)
   SET(SAVED_C_DEFAULT_FLAGS "${CMAKE_C_FLAGS}")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall -Wno-unused-function")
   if(NOT MSYS)
	include(CheckCCompilerFlag)
	CHECK_C_COMPILER_FLAG("-fvisibility=hidden" SUPPORT_VISIBILITY)
	if(SUPPORT_VISIBILITY)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	endif()
  endif()
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# The config files will end up in the binary dir
include_directories("${FMILibrary_BINARY_DIR}")

if(FMILIB_BUILD_FOR_SHARED_LIBS) 
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
	SET(WITH_PIC ON) # this is used in MERGE_LIBRARIES below
endif()

# prefix each element of list by ${prefix}element
macro(PREFIXLIST list_name prefix)
# create empty list - necessary?
	SET(${list_name}_TMP)

    # prefix and suffix elements
    foreach(l IN LISTS ${list_name})
      list(APPEND ${list_name}_TMP ${prefix}${l}${suffix} )
    endforeach()
	
    # replace list by tmp list
    SET(${list_name} ${${list_name}_TMP})
    UNSET(${list_name}_TMP)
endmacro(PREFIXLIST)

# For MSVC define source groups. Does not work for MSVC 8. MSVC 9 not tested.
if(MSVC AND (${CMAKE_GENERATOR} MATCHES "Visual Studio 10"))
	source_group("Source files\\FMI1" REGULAR_EXPRESSION "fmi1_.*.c") 
	source_group("Source files\\FMI2" REGULAR_EXPRESSION "fmi2_.*.c") 
	source_group("Header files\\FMI1" REGULAR_EXPRESSION "fmi1_.*.h") 
	source_group("Header files\\FMI2" REGULAR_EXPRESSION "fmi2_.*.h") 
	source_group("Private headers" REGULAR_EXPRESSION "fmi_.*impl.h") 
	source_group("Private headers\\FMI1" REGULAR_EXPRESSION "fmi1_.*impl.h") 
	source_group("Private headers\\FMI2" REGULAR_EXPRESSION "fmi2_.*impl.h") 
endif()

set(FMILIBKIND STATIC)
if(NOT FMILIB_STATIC_LIB_ONLY)
	add_definitions(-DFMILIB_BUILDING_LIBRARY)
endif()

include(CheckSymbolExists)
include(CheckFunctionExists)
CHECK_SYMBOL_EXISTS(va_copy stdarg.h HAVE_VA_COPY)
CHECK_SYMBOL_EXISTS(__va_copy stdarg.h HAVE___VA_COPY)

CHECK_SYMBOL_EXISTS(vsnprintf stdio.h HAVE_VSNPRINTF_SYM)
CHECK_FUNCTION_EXISTS(vsnprintf HAVE_VSNPRINTF_FCN)

MACRO(FMIL_EVAL_CONDITION var_to_store_cond_eval)
   IF(${ARGN})
     SET(${var_to_store_cond_eval} 1)
   ELSE(${ARGN})
     SET(${var_to_store_cond_eval} 0)
   ENDIF(${ARGN})
ENDMACRO(FMIL_EVAL_CONDITION)

FMIL_EVAL_CONDITION(HAVE_VSNPRINTF HAVE_VSNPRINTF_SYM OR HAVE_VSNPRINTF_FCN)

# MSVC < 19.0 (Visual Studio 2015) have a non-standard vsnprintf
# implementation. Use the one provided by c99snprintf instead
if (MSVC AND (MSVC_VERSION LESS 1900))
    set(HAVE_VSNPRINTF FALSE)
endif()

add_subdirectory(ThirdParty/c99_snprintf)

SET(CMAKE_MODULE_PATH ${FMILIBRARYHOME}/Config.cmake)

include(jmutil)
include(fmixml)
include(fmicapi)
include(fmizip)
include(fmiimport)

#Cmake variables set in config files.
if(WIN32)
	set(FMI_FILE_SEP "\\\\")
else(WIN32)
	set(FMI_FILE_SEP "/")
endif(WIN32)

set(FMILIB_FMI_PLATFORM_DOC "FMI platform defines the subdirectory within FMU where binary is located")
set(FMILIB_FMI_PLATFORM ”unknown” CACHE STRING ${FMILIB_FMI_PLATFORM_DOC})
if(FMILIB_FMI_PLATFORM MATCHES ”unknown”)
    include(fmiplatform)
    fmi_platform(FMI_PLATFORM)
	set(FMILIB_FMI_PLATFORM ${FMI_PLATFORM} CACHE STRING ${FMILIB_FMI_PLATFORM_DOC} FORCE)
else()
	set(FMI_PLATFORM ${FMILIB_FMI_PLATFORM})
endif()

configure_file (
  "${FMILibrary_SOURCE_DIR}/Config.cmake/config_fmilib.h.cmake"
  "${FMILibrary_BINARY_DIR}/fmilib_config.h"
  ) 

set(FMILIB_SHARED_SUBLIBS ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES} expat minizip zlib c99snprintf)
set(FMILIB_SUBLIBS ${FMIIMPORT_LIBRARIES} ${JMUTIL_LIBRARIES} ${FMILIB_SHARED_SUBLIBS})
set(FMILIB_SHARED_SRC ${FMIIMPORTSOURCE} ${JMUTILSOURCE} ${FMIIMPORTHEADERS})

if(FMILIB_GENERATE_BUILD_STAMP)
	debug_message("FMILIB_SUBLIBS= ${FMILIB_SUBLIBS}")
    add_custom_command(
		OUTPUT ${FMILibrary_BINARY_DIR}/config_fmilib.c
		COMMAND	"${CMAKE_COMMAND}" 
			-D FMILIBRARYHOME="${FMILIBRARYHOME}" -D FMILIBRARYBUILD="${FMILIBRARYBUILD}"
			-P ${FMILIBRARYHOME}/Config.cmake/config_stamp.cmake
		COMMAND ${CMAKE_COMMAND}  -E touch ${FMILibrary_BINARY_DIR}/config_fmilib.c
		DEPENDS ${FMILIB_SUBLIBS}
	)
	add_library(fmilib_timestamp STATIC ${FMILibrary_BINARY_DIR}/config_fmilib.c "${FMILibrary_BINARY_DIR}/fmilib_config.h")
#	add_dependencies(fmilib_timestamp ${FMILIB_SUBLIBS} expat minizip zlib ${FMILIBRARYHOME}/Config.cmake/config_stamp.cmake)
	
	set(FMILIB_SUBLIBS ${FMILIB_SUBLIBS} fmilib_timestamp)
	set(FMILIB_SHARED_SRC ${FMILIB_SHARED_SRC} ${FMILibrary_BINARY_DIR}/config_fmilib.c)
endif()

set_target_properties(
	${FMILIB_SUBLIBS}
    PROPERTIES FOLDER "Sublibs")
	
if(FMILIB_INSTALL_SUBLIBS)
	set(FMILIB_TARGETS fmiimport jmutils fmizip fmixml fmicapi)
endif()
	
if(FMILIB_BUILD_STATIC_LIB)
	include(mergestaticlibs)
	if(WIN32)
		merge_static_libs(fmilib ${FMILIB_SUBLIBS})
        target_link_libraries(fmilib Shlwapi)
        foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
			set(flags "")
			string(TOUPPER "STATIC_LIBRARY_FLAGS_${CONFIG_TYPE}" PROPNAME)
			get_target_property(flags fmilib ${PROPNAME})
			set_target_properties(fmilib PROPERTIES ${PROPNAME} "${flags} Shlwapi.lib")
		endforeach()
	else()
		merge_static_libs(fmilib ${FMILIB_SUBLIBS} )
	endif(WIN32)
	if(UNIX) 
		target_link_libraries(fmilib dl)
	endif(UNIX)
	set(FMILIB_TARGETS ${FMILIB_TARGETS} fmilib)
endif()

if(FMILIB_BUILD_SHARED_LIB)
	add_library(fmilib_shared SHARED ${FMILIB_SHARED_SRC})

    if (UNIX)
        target_compile_definitions(fmilib_shared PRIVATE -D_GNU_SOURCE)
    endif()

	target_link_libraries(fmilib_shared ${FMILIB_SHARED_SUBLIBS})
	set(FMILIB_TARGETS ${FMILIB_TARGETS} fmilib_shared)
endif()

if(FMILIB_BUILD_TESTS)
	include(runtime_test)
	configure_file (
		"${FMILibrary_SOURCE_DIR}/Config.cmake/config_test.h.cmake"
		"${FMILibrary_BINARY_DIR}/config_test.h"
	)
endif()

file(COPY "${FMILIBRARYHOME}/Config.cmake/fmilib.h" DESTINATION "${FMILibrary_BINARY_DIR}")

install(TARGETS ${FMILIB_TARGETS}
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION lib
)
install(FILES 
			"${FMILIBRARYHOME}/FMILIB_Readme.txt"
			"${FMILIBRARYHOME}/FMILIB_License.txt"
			"${FMILIBRARYHOME}/FMILIB_Acknowledgements.txt"
		DESTINATION doc)
		
install(FILES 
			"${FMILibrary_BINARY_DIR}/fmilib_config.h" 
			"${FMILibrary_BINARY_DIR}/fmilib.h" 
		DESTINATION include)

install(DIRECTORY 
	${FMIIMPORTDIR}/include 
	${JMUTILDIR}/include
	DESTINATION .
    FILES_MATCHING PATTERN "*.h")

if(FMILIB_INSTALL_SUBLIBS)
	install(DIRECTORY 
		${FMICAPIDIR}/include 	
		${FMIXMLDIR}/include 
		${FMIZIPDIR}/include
		DESTINATION .
        FILES_MATCHING PATTERN "*.h")
endif()
	
install(DIRECTORY 
	${FMILIB_FMI_STANDARD_HEADERS}/FMI1
	${FMILIB_FMI_STANDARD_HEADERS}/FMI2
	DESTINATION include
    FILES_MATCHING PATTERN "*.h")

function(append_file_context file_out file_in)
	#file(APPEND filename "message to write"... )
	#file(READ filename variable [LIMIT numBytes] [OFFSET offset] [HEX])
	file(READ "${file_in}" text)
	string(REGEX REPLACE "%.*%" " " text "${text}")	
	file(APPEND ${file_out} "${text}")
	set(text "")
endfunction()
	

if(FMILIB_GENERATE_DOXYGEN_DOC)

	file(MAKE_DIRECTORY "${FMILIB_INSTALL_PREFIX}/doc")

	set(DOXYFILE_IN ${FMILIBRARYHOME}/Config.cmake/fmilib_doxydoc.conf CACHE INTERNAL "Doxygen config file")
	set(DOXYFILE_IMAGE_DIR "${FMILIBRARYHOME}/images" CACHE INTERNAL "Doxygen images" FORCE)

	file(MAKE_DIRECTORY ${FMILIBRARYBUILD}/doc)
	
	file(COPY 
		"${FMILIBRARYHOME}/Config.cmake/fmilib.h"
		"${FMILibrary_BINARY_DIR}/fmilib_config.h"
		DESTINATION ${FMILIBRARYBUILD}/doc)

	set(fmilib_mainpage_h "${FMILIBRARYBUILD}/doc/fmilib_mainpage.h")
	file(WRITE ${fmilib_mainpage_h} "/** @file fmilib_mainpage.h \n @brief Autogenerated file with documentation. \n ")
	append_file_context(${fmilib_mainpage_h} "${FMILIBRARYHOME}/FMILIB_Readme.txt" )
	append_file_context(${fmilib_mainpage_h} "${FMILIBRARYHOME}/FMILIB_License.txt" )
	append_file_context(${fmilib_mainpage_h} "${FMILIBRARYHOME}/FMILIB_Acknowledgements.txt" )
	file(APPEND ${fmilib_mainpage_h} "*/")

	set(DOXYFILE_SOURCE_DIR "${FMILIBRARYBUILD}/doc" CACHE INTERNAL "Doxygen default source dir" FORCE)
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMILIBRARYHOME}/Test\"")

	set(DOXYFILE_OUTPUT_DIR "${FMILIB_INSTALL_PREFIX}/doc")

	set(DOXYFILE_STRIP_FROM_PATH "${FMILIBRARYHOME}")

	SET(CMAKE_MODULE_PATH ${FMILIB_THIRDPARTYLIBS}/CMakeModules)
 
	include(UseDoxygen/UseDoxygen)
endif(FMILIB_GENERATE_DOXYGEN_DOC)
