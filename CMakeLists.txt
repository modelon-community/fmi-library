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

cmake_minimum_required(VERSION 3.15)

# Prohibit in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source build is not supported. Please, use an empty directory for building the project.")
endif()

project(FMILibrary)

# Verify that float parsing will work for the platform/compiler config. Ignoring CHAR_BIT for exotic platforms.
include(CheckTypeSize)
check_type_size("double" SIZEOF_DOUBLE BUILTIN_TYPES_ONLY LANGUAGE "C")
if(NOT (${SIZEOF_DOUBLE} EQUAL "8" OR ${SIZEOF_DOUBLE} GREATER "8"))
    message(FATAL_ERROR "Required size of C type 'double': at least 64 bit. Return value of sizeof(double): ${SIZEOF_DOUBLE}")
endif()

set_property(GLOBAL PROPERTY C_STANDARD                         99)
set_property(GLOBAL PROPERTY C_STANDARD_REQUIRED                ON)
set_property(GLOBAL PROPERTY PROPERTYPREDEFINED_TARGETS_FOLDER  Global)
set(CMAKE_SOURCE_DIR ${FMILibrary_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${FMILibrary_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Config.cmake)

# User configuration options and parameters
set(FMILIB_THIRDPARTYLIBS  ${CMAKE_SOURCE_DIR}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )
set(FMILIB_FMI_STANDARD_HEADERS  ${FMILIB_THIRDPARTYLIBS}/FMI/default CACHE PATH "Path to the FMI standard headers dir" )

option(FMILIB_DEFAULT_BUILD_TYPE_RELEASE "Default build-type used for Makefile generation is 'Release'. Can be overwritten by CMAKE_BUILD_TYPE command line switch." ON)

if(MSVC)
    option (FMILIB_BUILD_WITH_STATIC_RTLIB "Use static run-time libraries (/MT or /MTd linker flags)" OFF)
endif()

option(FMILIB_BUILD_STATIC_LIB "Build the library as static." ON)
option(FMILIB_BUILD_SHARED_LIB "Build the library as shared (dll/so/dylib)." ON)
option(FMILIB_INSTALL_SUBLIBS  "Install sub-libraries (fmicapi. fmixml, jmutil, fmiimport, fmizip)" OFF)
option(FMILIB_SYSTEM_ZLIB  "Use find_package for zlib, instead of version in FMILibrary/Thirdparty." OFF)
option(FMILIB_SYSTEM_MINIZIP "Use system minizp library instead of the one in FMILibrary/Thirdparty. Static builds of FMI Library will not bundle minizip." OFF)
option(FMILIB_SYSTEM_EXPAT "Use system expat library instead of the one in FMILibrary/Thirdparty. Static builds of FMI Library will not bundle expat." OFF)
option(FMILIB_EXTERNAL_LIBS "Use system libraries" OFF)
if (FMILIB_EXTERNAL_LIBS)
    set(FMILIB_SYSTEM_ZLIB ON CACHE BOOL "Use system zlib library" FORCE)
    set(FMILIB_SYSTEM_MINIZIP ON CACHE BOOL "Use system minizp library" FORCE)
    set(FMILIB_SYSTEM_EXPAT ON CACHE BOOL "Use system expat library" FORCE)
endif()

if(NOT (WIN32 OR CYGWIN OR APPLE))
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

option(FMILIB_GENERATE_DOXYGEN_DOC "Generate doxygen doc target" ON)

option(FMILIB_BUILD_TESTS "Build tests" ON)
option(FMILIB_BUILD_BEFORE_TESTS "Force build before testing" ON)
option(FMILIB_LINK_TEST_TO_SHAREDLIB "Link the tests to fmilib_shared (if built) instead of fmilib" ON)

option(FMILIB_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
option(FMILIB_ENABLE_LOG_LEVEL_DEBUG "Enable log level 'debug'. If the option is of then the debug level is not compiled in." OFF)
option(FMILIB_PRINT_DEBUG_MESSAGES "Enable printing of status messages from the build script. Intended for debugging." OFF)
option(FMILIB_TEST_LOCALE          "Perform testing related to setting locales (requires certain language packs)" OFF)
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

if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${FMILIB_DEFAULT_BUILD_TYPE})
endif()

# debug_message is used to trace the build script
function(debug_message)
    if(FMILIB_PRINT_DEBUG_MESSAGES)
        message(STATUS "${ARGV}")
    endif()
endfunction()

if(NOT CMAKE_CFG_INTDIR)
    set(CMAKE_CFG_INTDIR ${FMILIB_DEFAULT_BUILD_TYPE})
endif()

if(MSVC)
    # With C89 we're forced to use non-secure functions
    # TODO: We're now using C99, and this can be cleaned up
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)

    # Set the run-time library flag as controlled by the FMILIB_BUILD_WITH_STATIC_RTLIB option
    if(FMILIB_BUILD_WITH_STATIC_RTLIB)
        set(replace_from "/MD")
        set(replace_to "/MT")
    else()
        set(replace_from "/MT")
        set(replace_to "/MD")
    endif(FMILIB_BUILD_WITH_STATIC_RTLIB)

    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)

        # Replace flags for static/shared lib
        if(${${flag_var}} MATCHES "${replace_from}")
            string(REPLACE "${replace_from}" "${replace_to}" tmp "${${flag_var}}")
            set(${flag_var} ${tmp} CACHE STRING "compiler flags" FORCE)
        endif()

    endforeach()
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    set(SAVED_C_DEFAULT_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -Wall -Wno-unused-function")
    if(NOT MSYS)
        include(CheckCCompilerFlag)
        CHECK_C_COMPILER_FLAG("-fvisibility=hidden" SUPPORT_VISIBILITY)
        if(SUPPORT_VISIBILITY)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        endif()
    endif()
endif()

if(FMILIB_BUILD_FOR_SHARED_LIBS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
    set(WITH_PIC ON) # this is used in MERGE_LIBRARIES below
endif()

# prefix each element of list by ${prefix}element
macro(PREFIXLIST list_name prefix)
# create empty list - necessary?
    set(${list_name}_TMP)

    # prefix and suffix elements
    foreach(l IN LISTS ${list_name})
        list(APPEND ${list_name}_TMP ${prefix}${l}${suffix} )
    endforeach()

    # replace list by tmp list
    set(${list_name} ${${list_name}_TMP})
    unset(${list_name}_TMP)
endmacro()

# For MSVC define source groups. Does not work for MSVC 8. MSVC 9 not tested.
if(MSVC AND (${CMAKE_GENERATOR} MATCHES "Visual Studio 10"))
    source_group("Source files\\FMI1" REGULAR_EXPRESSION "fmi1_.*.c")
    source_group("Source files\\FMI2" REGULAR_EXPRESSION "fmi2_.*.c")
    source_group("Source files\\FMI3" REGULAR_EXPRESSION "fmi3_.*.c")
    source_group("Header files\\FMI1" REGULAR_EXPRESSION "fmi1_.*.h")
    source_group("Header files\\FMI2" REGULAR_EXPRESSION "fmi2_.*.h")
    source_group("Header files\\FMI3" REGULAR_EXPRESSION "fmi3_.*.h")
    source_group("Private headers" REGULAR_EXPRESSION "fmi_.*impl.h")
    source_group("Private headers\\FMI1" REGULAR_EXPRESSION "fmi1_.*impl.h")
    source_group("Private headers\\FMI2" REGULAR_EXPRESSION "fmi2_.*impl.h")
    source_group("Private headers\\FMI3" REGULAR_EXPRESSION "fmi3_.*impl.h")
endif()

if(NOT FMILIB_STATIC_LIB_ONLY)
    add_definitions(-DFMILIB_BUILDING_LIBRARY)
endif()

include(CheckSymbolExists)
include(CheckFunctionExists)
CHECK_SYMBOL_EXISTS(va_copy stdarg.h HAVE_VA_COPY)
CHECK_SYMBOL_EXISTS(__va_copy stdarg.h HAVE___VA_COPY)

CHECK_SYMBOL_EXISTS(vsnprintf stdio.h HAVE_VSNPRINTF_SYM)
CHECK_FUNCTION_EXISTS(vsnprintf HAVE_VSNPRINTF_FCN)

macro(FMIL_EVAL_CONDITION var_to_store_cond_eval)
    if(${ARGN})
        set(${var_to_store_cond_eval} 1)
    else()
        set(${var_to_store_cond_eval} 0)
    endif()
endmacro()

FMIL_EVAL_CONDITION(HAVE_VSNPRINTF HAVE_VSNPRINTF_SYM OR HAVE_VSNPRINTF_FCN)

# MSVC < 19.0 (Visual Studio 2015) have a non-standard vsnprintf
# implementation. Use the one provided by c99snprintf instead
if(MSVC AND (MSVC_VERSION LESS 1900))
    set(HAVE_VSNPRINTF FALSE)
endif()

#Cmake variables set in config files.
if(WIN32)
    set(FMI_FILE_SEP "\\\\")
else()
    set(FMI_FILE_SEP "/")
endif()

# Sets VARNAME to the user-configured platform name, or auto-detects it based on the
# current platform and provided FMI version.
macro(set_fmi_platform VARNAME FMI_VERSION)
    set(FMILIB_FMI_PLATFORM_DOC "The subdirectory in the FMU's binaries directory where the shared library is located")
    set(FMILIB_FMI${FMI_VERSION}_PLATFORM unknown CACHE STRING ${FMILIB_FMI_PLATFORM_DOC})
    if(FMILIB_FMI${FMI_VERSION}_PLATFORM MATCHES unknown)
        include(fmiplatform)
        fmi_platform(${VARNAME} ${FMI_VERSION})
        set(FMILIB_FMI${FMI_VERSION}_PLATFORM ${${VARNAME}} CACHE STRING ${FMILIB_FMI_PLATFORM_DOC} FORCE)
    else()
        set(${VARNAME} ${FMILIB_FMI${FMI_VERSION}_PLATFORM})
    endif()
endmacro()
set_fmi_platform(FMI1_PLATFORM 1)
set_fmi_platform(FMI2_PLATFORM 2)
set_fmi_platform(FMI3_PLATFORM 3)

# ===============================================================================
# Generate headers
# ===============================================================================

# Put the headers in different directories to allow more fine-grained include flags,
# since we don't want to accidentally include more than we should. That could lead
# to issues not detected until testing the installation.
set(FMILIB_CONFIG_INCLUDE_DIR      "${CMAKE_BINARY_DIR}/fmilib_config_include")
set(FMILIB_TESTCONFIG_INCLUDE_DIR  "${CMAKE_BINARY_DIR}/fmilib_test_config_include")
set(FMILIB_INCLUDE_DIR             "${CMAKE_BINARY_DIR}/fmilib_include")

file(MAKE_DIRECTORY "${FMILIB_CONFIG_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${FMILIB_TESTCONFIG_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${FMILIB_INCLUDE_DIR}")

configure_file (
    "${CMAKE_SOURCE_DIR}/Config.cmake/config_fmilib.h.cmake"
    "${FMILIB_CONFIG_INCLUDE_DIR}/fmilib_config.h"
)

file(COPY "${CMAKE_SOURCE_DIR}/Config.cmake/fmilib.h" DESTINATION "${FMILIB_INCLUDE_DIR}")

# ===============================================================================

if(MSVC AND (MSVC_VERSION LESS 1900))
    add_subdirectory(ThirdParty/c99_snprintf)
endif()

include(jmutil)
include(fmixml)
include(fmicapi)
include(fmizlib)
include(fmizip)
include(fmiimport)
if(FMILIB_BUILD_TESTS)
    include(runtime_test)
    # Configuration depends on values set in all the different test*.cmake files,
    # so can't be done until here.
    configure_file (
        "${CMAKE_SOURCE_DIR}/Config.cmake/config_test.h.cmake"
        "${FMILIB_TESTCONFIG_INCLUDE_DIR}/config_test.h"
    )
endif()

set(FMILIB_SHARED_SUBLIBS fmixml fmizip fmicapi)
if (NOT FMILIB_SYSTEM_MINIZIP)
    list(APPEND FMILIB_SHARED_SUBLIBS minizip)
endif ()
if (NOT FMILIB_SYSTEM_ZLIB)
    list(APPEND FMILIB_SHARED_SUBLIBS zlib)
endif ()
if(NOT FMILIB_SYSTEM_EXPAT)
    list(APPEND FMILIB_SHARED_SUBLIBS expat)
endif()
if(MSVC AND (MSVC_VERSION LESS 1900))
    list(APPEND FMILIB_SHARED_SUBLIBS c99snprintf)
endif()
set(FMILIB_SUBLIBS fmiimport jmutils ${FMILIB_SHARED_SUBLIBS})
# XXX: Unclear why we create intermediate sublibs instead of just giving everything as source.
# Probably also wouldn't get problems with using PRIVATE/PUBLIC on linking sublibs on Windows.
set(FMILIB_SHARED_SRC ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS} ${JMUTILSOURCE} ${JMUTIL_PUBHEADERS})

if(FMILIB_GENERATE_BUILD_STAMP)
    debug_message("FMILIB_SUBLIBS= ${FMILIB_SUBLIBS}")
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/config_fmilib.c
        COMMAND    "${CMAKE_COMMAND}"
            -D CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" -D CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
            -P ${CMAKE_SOURCE_DIR}/Config.cmake/config_stamp.cmake
        COMMAND ${CMAKE_COMMAND}  -E touch ${CMAKE_BINARY_DIR}/config_fmilib.c
        DEPENDS ${FMILIB_SUBLIBS}
    )
    add_library(fmilib_timestamp STATIC ${CMAKE_BINARY_DIR}/config_fmilib.c "${FMILIB_CONFIG_INCLUDE_DIR}/fmilib_config.h")
    target_include_directories(fmilib_timestamp PRIVATE ${FMILIB_CONFIG_INCLUDE_DIR})

    set(FMILIB_SUBLIBS ${FMILIB_SUBLIBS} fmilib_timestamp)
    set(FMILIB_SHARED_SRC ${FMILIB_SHARED_SRC} ${CMAKE_BINARY_DIR}/config_fmilib.c)
endif()

set_target_properties(
    ${FMILIB_SUBLIBS}
    PROPERTIES FOLDER "Sublibs"
)

if(FMILIB_INSTALL_SUBLIBS)
    set(FMILIB_TARGETS fmiimport jmutils fmizip fmixml fmicapi)
endif()

if(FMILIB_BUILD_STATIC_LIB)
    include(mergestaticlibs)
    merge_static_libs(fmilib ${FMILIB_SUBLIBS})
    if(WIN32)
        target_link_libraries(fmilib PUBLIC shlwapi)
    endif()
    if(UNIX)
        target_link_libraries(fmilib PUBLIC dl)
    endif()
    set(FMILIB_TARGETS ${FMILIB_TARGETS} fmilib)
    # Set PUBLIC include_directories such that test targets that link with fmilib inherit
    # the same include dirs as will exist for installation.
    target_include_directories(fmilib
        PUBLIC
            ${JMUTIL_PUBLIC_INCLUDE_DIR}
            ${FMIIMPORT_PUBLIC_INCLUDE_DIR}
            ${FMILIB_INCLUDE_DIR}
            ${FMILIB_CONFIG_INCLUDE_DIR}
    )
endif()

if(FMILIB_BUILD_SHARED_LIB)
    add_library(fmilib_shared SHARED ${FMILIB_SHARED_SRC})

    # Even if fmilib_shared is built from source files, it's still necessary that e.g.
    # fmiimport's dependencies such as generated code is built first.
    add_dependencies(fmilib_shared fmiimport)

    if(UNIX AND NOT APPLE)
        target_compile_definitions(fmilib_shared PRIVATE -D_GNU_SOURCE)
    endif()

    # Must be PRIVATE, or tests will later inherit compile/link properties from sublibs.
    # That is incorrect, and may cause link issues.
    #
    # FIXME:
    # Issue: We link static libraries (fmixml) that depend on other static libraries (jmutils),
    # which are also compiled from source (jmutils) to a shared library.
    # One solution is to also package jmutils as a shared lib and to fix proper packaging for
    # find_package etc. (which would make it easier to get away from merge_static_libs as well).
    target_link_libraries(fmilib_shared PRIVATE ${FMILIB_SHARED_SUBLIBS} ${JMUTILS_DEPENDENCY_LIBS})

    target_include_directories(fmilib_shared
        PRIVATE
            ${FMIIMPORT_PRIVATE_INCLUDE_DIRS}
        PUBLIC
            ${JMUTILS_PUBLIC_INCLUDE_DIRS}
            ${FMIIMPORT_PUBLIC_INCLUDE_DIRS}
            ${FMILIB_INCLUDE_DIR}
            ${FMILIB_CONFIG_INCLUDE_DIR}
    )
    set(FMILIB_TARGETS ${FMILIB_TARGETS} fmilib_shared)
endif()

include (GNUInstallDirs)

install(TARGETS ${FMILIB_TARGETS}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES
    "${CMAKE_SOURCE_DIR}/FMILIB_Readme.txt"
    "${CMAKE_SOURCE_DIR}/FMILIB_License.txt"
    "${CMAKE_SOURCE_DIR}/FMILIB_Acknowledgements.txt"
    DESTINATION ${CMAKE_INSTALL_DOCDIR})

install(FILES
    "${FMILIB_CONFIG_INCLUDE_DIR}/fmilib_config.h"
    "${FMILIB_INCLUDE_DIR}/fmilib.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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
    ${FMILIB_FMI_STANDARD_HEADERS}/FMI3
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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

    set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Config.cmake/fmilib_doxydoc.conf CACHE INTERNAL "Doxygen config file")
    set(DOXYFILE_IMAGE_DIR "${CMAKE_SOURCE_DIR}/images" CACHE INTERNAL "Doxygen images" FORCE)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc)

    file(COPY
        "${CMAKE_SOURCE_DIR}/Config.cmake/fmilib.h"
        "${FMILIB_CONFIG_INCLUDE_DIR}/fmilib_config.h"
        DESTINATION ${CMAKE_BINARY_DIR}/doc)

    set(fmilib_mainpage_h "${CMAKE_BINARY_DIR}/doc/fmilib_mainpage.h")
    file(WRITE ${fmilib_mainpage_h} "/** @file fmilib_mainpage.h \n @brief Autogenerated file with documentation. \n ")
    append_file_context(${fmilib_mainpage_h} "${CMAKE_SOURCE_DIR}/FMILIB_Readme.txt" )
    append_file_context(${fmilib_mainpage_h} "${CMAKE_SOURCE_DIR}/FMILIB_License.txt" )
    append_file_context(${fmilib_mainpage_h} "${CMAKE_SOURCE_DIR}/FMILIB_Acknowledgements.txt" )
    file(APPEND ${fmilib_mainpage_h} "*/")

    set(DOXYFILE_SOURCE_DIR "${CMAKE_BINARY_DIR}/doc" CACHE INTERNAL "Doxygen default source dir" FORCE)
    set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${CMAKE_SOURCE_DIR}/Test\"")

    set(DOXYFILE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/doc")

    set(DOXYFILE_STRIP_FROM_PATH "${CMAKE_SOURCE_DIR}")

    set(CMAKE_MODULE_PATH ${FMILIB_THIRDPARTYLIBS}/CMakeModules)

    include(UseDoxygen/UseDoxygen)
endif(FMILIB_GENERATE_DOXYGEN_DOC)

# Debug only: prints the final value of all the non-environment variables
if(FMILIB_DEBUG_PRINT_VARIABLES_ONLY)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
