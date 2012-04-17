cmake_minimum_required (VERSION 2.8)
project (FMILibrary)

set(FMILIBRARYHOME ${FMILibrary_SOURCE_DIR})
set(THIRDPARTYLIBS  ${FMILibrary_SOURCE_DIR}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )

SET(FMILIBRARY_INSTALL_PREFIX ${FMILibrary_BINARY_DIR}/../install CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX ${FMILIBRARY_INSTALL_PREFIX} CACHE INTERNAL "Prefix prepended to install directories" FORCE)

include_directories("${FMILibrary_BINARY_DIR}")
  
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

#IF(CMAKE_CFG_INTDIR MATCHES "Debug")
#...Debug PART...
#ELSE(CMAKE_CFG_INTDIR MATCHES "Debug")
#...Release PART...
#ENDIF(CMAKE_CFG_INTDIR MATCHES "Debug")

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

source_group("Private headers" REGULAR_EXPRESSION ".*impl.h")

if(MSVC)
	set(BUILD_WITH_STATIC_CRT NO)
	ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${FMILIBRARYHOME}/Config.cmake)

include(jmutil)
include(fmixml)
include(fmicapi)
include(fmizip)
include(fmiimport)
include(runtime_test)

configure_file (
  "${FMILibrary_SOURCE_DIR}/Config.cmake/config.h.cmake"
  "${FMILibrary_BINARY_DIR}/config.h"
  )
  
install(TARGETS fmiimport jmutils fmizip fmixml fmicapi
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

install(DIRECTORY 
	${FMIIMPORTDIR}/include 
	${FMICAPIDIR}/include 
	${FMIXMLDIR}/include
	${FMIZIPDIR}/include # not sure if needed at install location
	${JMUTILDIR}/include
	DESTINATION .)

option (GENERATE_DOXYGEN_DOC "Generate doxygen doc target" ON)
if(GENERATE_DOXYGEN_DOC)
	set(DOXYFILE_IN ${FMILibrary_SOURCE_DIR}/Config.cmake/fmilib_doxydoc.conf CACHE INTERNAL "Doxygen config file")

	set(DOXYFILE_SOURCE_DIR "${FMILIBRARYHOME}/include" CACHE INTERNAL "Doxygen default source dir" FORCE)

	set(DOXYFILE_STRIP_FROM_PATH "${FMILIBRARYHOME}")
 
	include(UseDoxygen/UseDoxygen)
endif(GENERATE_DOXYGEN_DOC)
