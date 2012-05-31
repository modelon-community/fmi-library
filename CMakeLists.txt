cmake_minimum_required (VERSION 2.8)
project (FMILibrary)

set(FMILIBRARYHOME ${FMILibrary_SOURCE_DIR})
set(FMILIBRARYBUILD ${FMILibrary_BINARY_DIR})

# User configuration options and parameters
SET(FMILIB_INSTALL_PREFIX ${FMILibrary_BINARY_DIR}/../install CACHE PATH "Prefix prepended to install directories")
set(FMILIB_THIRDPARTYLIBS  ${FMILibrary_SOURCE_DIR}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )

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
option(FMILIB_LINK_TEST_TO_SHAREDLIB "Link the tests to fmilib_shared (if built) instead of fmilib" ON)

option(FMILIB_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
option(FMILIB_DEBUG_TRACE "Enable priting of trace messages from fmilib. Intended for debugging." OFF)
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

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${FMILIBRARYHOME}/Config.cmake)
include(libutils/libutils)
set(MYSQL_CMAKE_SCRIPT_DIR ${FMILIBRARYHOME}/Config.cmake/libutils)

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

source_group("Private headers" REGULAR_EXPRESSION ".*impl.h") # mostly for MSVC

set(FMILIBKIND STATIC)
if(NOT FMILIB_STATIC_LIB_ONLY)
	add_definitions(-DFMILIB_BUILDING_LIBRARY)
endif()

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
	if(CMAKE_HOST_WIN32) #Set to true when the host system is Windows and on cygwin.
		if(CMAKE_CL_64)
		set(FMI_PLATFORM win64)
		else(CMAKE_CL_64)
		set(FMI_PLATFORM win32)
		endif(CMAKE_CL_64)
	elseif(CMAKE_HOST_APPLE) #Set to true when the host system is Apple OSX. 
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(FMI_PLATFORM darwin32)
		else(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(FMI_PLATFORM darwin64)
		endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	elseif(CMAKE_HOST_UNIX) #Set to true when the host system is UNIX or UNIX like (i.e. APPLE and CYGWIN). 
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(FMI_PLATFORM linux32)
		else(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(FMI_PLATFORM linux64)
		endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	else(CMAKE_HOST_WIN32)
		message(SEND_ERROR "Target platform is not supported by the tests")
		set(FMI_PLATFORM not_supported)
	endif()
	
	set(FMILIB_FMI_PLATFORM ${FMI_PLATFORM} CACHE STRING ${FMILIB_FMI_PLATFORM_DOC} FORCE)
else()
	set(FMI_PLATFORM ${FMILIB_FMI_PLATFORM})
endif()

configure_file (
  "${FMILibrary_SOURCE_DIR}/Config.cmake/config_fmilib.h.cmake"
  "${FMILibrary_BINARY_DIR}/config_fmilib.h"
  )
  


set(FMILIB_SHARED_SUBLIBS ${FMIXML_LIBRARIES} ${FMIZIP_LIBRARIES} ${FMICAPI_LIBRARIES} expat minizip zlib)
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
	add_library(fmilib_timestamp STATIC ${FMILibrary_BINARY_DIR}/config_fmilib.c "${FMILibrary_BINARY_DIR}/config_fmilib.h")
#	add_dependencies(fmilib_timestamp ${FMILIB_SUBLIBS} expat minizip zlib ${FMILIBRARYHOME}/Config.cmake/config_stamp.cmake)
	
	set(FMILIB_SUBLIBS ${FMILIB_SUBLIBS} fmilib_timestamp)
	set(FMILIB_SHARED_SRC ${FMILIB_SHARED_SRC} ${FMILibrary_BINARY_DIR}/config_fmilib.c)
endif()

if(FMILIB_INSTALL_SUBLIBS)
	set(FMILIB_TARGETS fmiimport jmutils fmizip fmixml fmicapi)
endif()
	
if(FMILIB_BUILD_STATIC_LIB)
	MERGE_LIBRARIES( fmilib STATIC  NOINSTALL ${FMILIB_SUBLIBS})
	set(FMILIB_TARGETS ${FMILIB_TARGETS} fmilib)
endif()

if(FMILIB_BUILD_SHARED_LIB)
	add_library(fmilib_shared SHARED ${FMILIB_SHARED_SRC})
	target_link_libraries(fmilib_shared LINK_INTERFACE_LIBRARIES)
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
			"${FMILibrary_BINARY_DIR}/config_fmilib.h" 
			"${FMILibrary_BINARY_DIR}/fmilib.h" 
		DESTINATION include)

install(DIRECTORY 
	${FMIIMPORTDIR}/include 
	${FMICAPIDIR}/include 
	${FMIXMLDIR}/include
	${FMIZIPDIR}/include # not sure if needed at install location
	${JMUTILDIR}/include
	DESTINATION .)
	
install(DIRECTORY 
	${FMILIB_THIRDPARTYLIBS}/FMI/1.0-CS
	${FMILIB_THIRDPARTYLIBS}/FMI/1.0-ME	
	DESTINATION include)
	

if(FMILIB_GENERATE_DOXYGEN_DOC)

	file(MAKE_DIRECTORY "${FMILIB_INSTALL_PREFIX}/doc")

	set(DOXYFILE_IN ${FMILIBRARYHOME}/Config.cmake/fmilib_doxydoc.conf CACHE INTERNAL "Doxygen config file")

	set(DOXYFILE_SOURCE_DIR "${FMILIBRARYHOME}/Config.cmake" CACHE INTERNAL "Doxygen default source dir" FORCE)
	
	set(DOXYFILE_OUTPUT_DIR "${FMILIB_INSTALL_PREFIX}/doc")

	set(DOXYFILE_STRIP_FROM_PATH "${FMILIBRARYHOME}")
 
	include(UseDoxygen/UseDoxygen)
endif(FMILIB_GENERATE_DOXYGEN_DOC)
