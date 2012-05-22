/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
	* \mainpage FMI Import library (part of JModelica.org)
	*
	* \version     0.1a
	* \date April  2012
	* \section Summary
	The library is intended as a foundation for applications interfacing FMUs (Functional Mockup Units)
	that follow FMI Standard. See <http://functional-mockup-interface.org/> 

	* \section config Configuring and building
	CMake (see <http://www.cmake.org/>) is used to generate the native build scripts for the library.
	It is recommended to use "cmake-gui" on Windows or "ccmake" to configure the build.
	The primary targets of the library build script are:
	- <prefix>/include/fmilib.h
	  The include file to be used in client applications.
	- Library files under <prefix>/lib/
	  Static library is named 'fmilib' and shared library 'fmilib_shared'. The prefix/suffix of the library files 
	  differ depending on the platform. Note that if you have configure and built both static and shared library on 
	  Windows but want to link with the static library compile time define "FMILIB_BUILDING_LIBRARY" must be set.
	- Doxygen generated documentation under <prefix>/doc/

	The following build configuration options are provided:
	- FMILIB_INSTALL_PREFIX - prefix prepended to install directories.
	
	   This is the main install directory name. Include files will be located in the "include" subdirectory
	   and library files in the "lib" subdirectory. Client applications should only include "fmilib.h"

	- FMILIB_THIRDPARTYLIBS - thirdparty libaries are currently shipped with the library. 

	- FMILIB_DEFAULT_BUILD_TYPE_RELEASE - Controls build-type used for Makefile generation.
	
	  If this option is on then 'Release' mode compile flags are used. Otherwize, 'Debug' mode flags
	  are generated into the Makefile. The option may be overwritten by explicitly setting CMAKE_BUILD_TYPE.

	- FMILIB_BUILD_WITH_STATIC_RTLIB Use static run-time libraries (/MT or /MTd code generation flags).
	
	  This is only used when generating Microsoft Visual Studio solutions. If the options is on then the
	  library will be built against static runtime, otherwise - dynamic runtime (/MD or /MDd). Make sure
	  the client code is using matching runtime.

	- FMILIB_BUILD_STATIC_LIB Build the library as static.
	  'fmilib' may be used for static linking.
	  
	- FMILIB_BUILD_SHARED_LIB Build the library as shared (dll/so/dylib).
	  'fmilib_shared' may be used for dynamic linking.

	- FMILIB_BUILD_FOR_SHARED_LIBS  The static library 'fmilib' can be linked into shared libraries.

	  On LINUX position independent code (-fPIC) must be used on all files to be linked into a shared 
	  library (.so file). On other systems this is not needed (either is default or relocation is done).
	  Set this option to OFF if you are building an application on Linux and use static library only.

	- FMILIB_GENERATE_DOXYGEN_DOC Enable doxygen target.

	  The build system will automatically detect if you have doxygen installed. In this case this
	  option will become available.

	- FMILIB_BUILD_TESTS Enable build of the tests supplied with the library.

	  RUN_TESTS - target will be provided in Visual Studio. 'make test' will run tests on Makefile based platforms.

	- FMILIB_LINK_TEST_TO_SHAREDLIB Link the tests to fmilib_shared (if built) instead of static fmilib.

	- FMILIB_GENERATE_BUILD_STAMP Generate a build time stamp and include in into the library. 

	  The function fmilib_get_build_stamp may be used to retrieve the time stamp.
		    const char* fmilib_get_build_stamp(void);

	* \section License	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	* \copyright (C) 2012 Modelon AB
*/

/** \file fmilib.h
*  \brief Include file to be used in client applications of the FMI Library.
*/

#ifndef FMILIB_H_
#define FMILIB_H_

#include <stddef.h>
#include "config_fmilib.h"

#include <Common/fmi_import_context.h>
#include <FMI1/fmi1_import.h>
#endif
