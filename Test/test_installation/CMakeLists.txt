#    Copyright (C) 2023-2024 Modelon AB
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

# Performs a smoke test on the install directory of FMIL. Verifies for example
# that:
#   - All include files have been installed
#   - No XML headers etc. are included in the import headers
#   - That libraries are present and functions can be called
project(TestInstallation C)

set_property(GLOBAL PROPERTY C_STANDARD          99)
set_property(GLOBAL PROPERTY C_STANDARD_REQUIRED ON)

set(FMIL_HOME "unset" CACHE FILEPATH "Path to FMIL installation")

message(STATUS "FMIL_HOME: ${FMIL_HOME}")
if (FMIL_HOME STREQUAL "unset" OR FMIL_HOME STREQUAL "")
    message(FATAL_ERROR "Unset config variable: FMIL_HOME")
endif()

# Macro that sets MSVC RTL for the target to the same one as FMIL was compiled with.
macro(target_set_msvcrt TARGET_NAME)
    if(FMILIB_BUILD_WITH_STATIC_RTLIB)
        set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    else()
        set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()
endmacro()

enable_testing()

# ==============================================================================
# Test static installation
# ==============================================================================

# Import FMIL
add_library(fmilib STATIC IMPORTED)
set_property(TARGET fmilib PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${FMIL_HOME}/include")
set_property(TARGET fmilib PROPERTY IMPORTED_LOCATION "${FMIL_HOME}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}fmilib${CMAKE_STATIC_LIBRARY_SUFFIX}")

# Build the test
add_executable(main main.c)
if(WIN32)
    target_link_libraries(main PRIVATE fmilib shlwapi)
else()
    target_link_libraries(main PRIVATE fmilib dl)
endif()
if(WIN32)
    # Needed for compiling against static libs. FIXME: Should not be needed.
    target_compile_definitions(main PRIVATE FMILIB_STATIC_LIB_ONLY)
endif()
if(MSVC)
    target_set_msvcrt(main)
endif()

add_test(test_installation main)

# ==============================================================================
# Test shared installation
# ==============================================================================

# Import FMIL
add_library(fmilib_shared SHARED IMPORTED)
set_property(TARGET fmilib_shared PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${FMIL_HOME}/include")
set_property(TARGET fmilib_shared PROPERTY IMPORTED_LOCATION "${FMIL_HOME}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}fmilib_shared${CMAKE_SHARED_LIBRARY_SUFFIX}")
set_property(TARGET fmilib_shared PROPERTY IMPORTED_IMPLIB "${FMIL_HOME}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}fmilib_shared${CMAKE_IMPORT_LIBRARY_SUFFIX}")

# Build the test
add_executable(main_shared main.c)
if(WIN32)
    target_link_libraries(main_shared PRIVATE fmilib_shared shlwapi)
else()
    target_link_libraries(main_shared PRIVATE fmilib_shared dl)
endif()
if(MSVC)
    target_set_msvcrt(main_shared)
endif()

add_test(test_installation_shared main_shared)
if(WIN32)
    # NOTE: Just replacing PATH instead of appending since that's troublesome
    # due to escaping of ';' in Windows paths.
    set_property(TEST test_installation_shared PROPERTY ENVIRONMENT "PATH=${FMIL_HOME}/bin")
endif()
