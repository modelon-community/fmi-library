cmake_minimum_required (VERSION 2.8)
project (FMILibrary)

set(FMILIBRARYHOME ${FMILibrary_SOURCE_DIR})
set(FMILIBRARYBUILD ${FMILibrary_BINARY_DIR})

set(THIRDPARTYLIBS  ${FMILibrary_SOURCE_DIR}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )

SET(FMILIB_INSTALL_PREFIX ${FMILibrary_BINARY_DIR}/../install CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX ${FMILIB_INSTALL_PREFIX} CACHE INTERNAL "Prefix prepended to install directories" FORCE)

option(PRINT_DEBUG_MESSAGES OFF)
mark_as_advanced(FORCE PRINT_DEBUG_MESSAGES)
function(debug_message)
	if(PRINT_DEBUG_MESSAGES)
		message(STATUS "${ARGV}")
	endif()
endfunction()

include(CMakeParseArguments)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${FMILIBRARYHOME}/Config.cmake)
include(libutils/libutils)
set(MYSQL_CMAKE_SCRIPT_DIR ${FMILIBRARYHOME}/Config.cmake/libutils)

if(MSVC)
    
	ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
	
	option (FMILIB_BUILD_WITH_STATIC_RTLIB "Use static run-time libraries (/MT or /MTd linker flags)" OFF)
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

include_directories("${FMILibrary_BINARY_DIR}")
  
option(FMILIB_BUILD_FOR_SHARED_LIBS "The library can be linked into shared libraries" ON)

if(FMILIB_BUILD_FOR_SHARED_LIBS) 
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
	SET(WITH_PIC ON)
endif()

IF(CMAKE_COMPILER_IS_GNUCC)
   SET(SAVED_C_DEFAULT_FLAGS "${CMAKE_C_FLAGS}")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(NOT CMAKE_BUILD_TYPE)
 SET(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(NOT CMAKE_CFG_INTDIR)
 SET(CMAKE_CFG_INTDIR "Debug")
ENDIF(NOT CMAKE_CFG_INTDIR)

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

option(FMILIB_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
if(FMILIB_GENERATE_BUILD_STAMP)
include(config_stamp)
add_custom_target(fmilib_update_timestamp
	COMMAND	"${CMAKE_COMMAND}" 
		-D FMILIBRARYHOME="${FMILIBRARYHOME}" -D FMILIBRARYBUILD="${FMILIBRARYBUILD}"
		-P ${FMILIBRARYHOME}/Config.cmake/config_stamp.cmake
	WORKING_DIRECTORY ${FMILibrary_BINARY_DIR}
)
add_library(fmilib_timestamp STATIC ${FMILibrary_BINARY_DIR}/config_fmilib.c "${FMILibrary_BINARY_DIR}/config_fmilib.h")
add_dependencies(fmilib_timestamp fmilib_update_timestamp)
endif()

source_group("Private headers" REGULAR_EXPRESSION ".*impl.h")
include(jmutil)
include(fmixml)
include(fmicapi)
include(fmizip)
include(fmiimport)
include(runtime_test)

#Cmake variables set in config files.
if(WIN32)
set(FMI_FILE_SEP "\\\\")
else(WIN32)
set(FMI_FILE_SEP "/")
endif(WIN32)

configure_file (
  "${FMILibrary_SOURCE_DIR}/Config.cmake/config_fmilib.h.cmake"
  "${FMILibrary_BINARY_DIR}/config_fmilib.h"
  )

  
configure_file (
  "${FMILibrary_SOURCE_DIR}/Config.cmake/config_test.h.cmake"
  "${FMILibrary_BINARY_DIR}/config_test.h"
  )

set(${CMAKE_SHARED_MODULE_SUFFIX})

set(FMILIB_SUBLIBS fmiimport jmutils fmizip fmixml fmicapi expat zlib minizip)
if(FMILIB_GENERATE_BUILD_STAMP)
	set(FMILIB_SUBLIBS ${FMILIB_SUBLIBS} fmilib_timestamp)
endif()

MERGE_LIBRARIES( fmilib STATIC  NOINSTALL ${FMILIB_SUBLIBS})
#MERGE_LIBRARIES( fmilib_shared SHARED NOINSTALL ${FMILIB_SUBLIBS} )
#add_library(fmilib ${FMILibrary_BINARY_DIR}/config_fmilib.c)

#target_link_libraries(fmilib fmiimport)
# fmilib fmilib_shared 
install(TARGETS fmiimport jmutils fmizip fmixml fmicapi fmilib
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(FILES "${FMILibrary_BINARY_DIR}/config_fmilib.h" DESTINATION include)

install(DIRECTORY 
	${FMIIMPORTDIR}/include 
	${FMICAPIDIR}/include 
	${FMIXMLDIR}/include
	${FMIZIPDIR}/include # not sure if needed at install location
	${JMUTILDIR}/include
	DESTINATION .)	
install(DIRECTORY 
	${THIRDPARTYLIBS}/FMI/1.0-CS
	${THIRDPARTYLIBS}/FMI/1.0-ME	
	DESTINATION include)
	

option (FMILIB_GENERATE_DOXYGEN_DOC "Generate doxygen doc target" ON)
if(FMILIB_GENERATE_DOXYGEN_DOC)
	set(DOXYFILE_IN ${FMILibrary_SOURCE_DIR}/Config.cmake/fmilib_doxydoc.conf CACHE INTERNAL "Doxygen config file")

	set(DOXYFILE_SOURCE_DIR "${FMILIBRARYHOME}/include" CACHE INTERNAL "Doxygen default source dir" FORCE)

	set(DOXYFILE_STRIP_FROM_PATH "${FMILIBRARYHOME}")
 
	include(UseDoxygen/UseDoxygen)
endif(FMILIB_GENERATE_DOXYGEN_DOC)
