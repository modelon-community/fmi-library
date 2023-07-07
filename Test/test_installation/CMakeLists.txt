#    Copyright (C) 2023 Modelon AB
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
 
cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)

# Performs a smoke test on the install directory of FMIL. Verifies for example
# that:
#   - All include files have been installed
#   - No XML headers etc. are included in the import headers
#   - That libraries are present and functions can be called
project(TestInstallation)

set_property(GLOBAL PROPERTY C_STANDARD          99)
set_property(GLOBAL PROPERTY C_STANDARD_REQUIRED ON)

set(FMIL_HOME "unset" CACHE FILEPATH "Path to FMIL installation")

message(STATUS "FMIL_HOME: ${FMIL_HOME}")
if (FMIL_HOME STREQUAL "unset" OR FMIL_HOME STREQUAL "")
    message(FATAL_ERROR "Unset config variable: FMIL_HOME")
endif()

add_executable(main main.c)
target_link_directories(main PRIVATE ${FMIL_HOME}/lib)
target_include_directories(main PRIVATE ${FMIL_HOME}/include)

add_test(test_fmil_installation main)
