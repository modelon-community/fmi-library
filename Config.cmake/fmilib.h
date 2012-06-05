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
	* \version     1.0.x
	* \date April  2012
	* \section Summary
	The library is intended as a foundation for applications interfacing FMUs (Functional Mockup Units)
	that follow FMI Standard. See <http://functional-mockup-interface.org/> 

	* \section config Configuring and building
	CMake (see <http://www.cmake.org/>) is used to generate the native build scripts for the library.
	It is recommended to use "cmake-gui" on Windows or "ccmake" to configure the build.
	The primary targets of the library build script are:
	- <prefix>/include/fmilib.h <br>
	  The include file to be used in client applications.
	- Library files under <prefix>/lib/ <br>
	  Static library is named 'fmilib' and shared library 'fmilib_shared'. The prefix/suffix of the library files 
	  differ depending on the platform. Note that if you have configure and built both static and shared library on 
	  Windows but want to link with the static library compile time define "FMILIB_BUILDING_LIBRARY" must be set.
	- Doxygen generated documentation under <prefix>/doc/

	The following build configuration options are provided:
	- \b FMILIB_INSTALL_PREFIX - prefix prepended to install directories.<br>	
	   This is the main install directory name. Include files will be located in the "include" subdirectory
	   and library files in the "lib" subdirectory. Client applications should only include "fmilib.h"

	- \b FMILIB_THIRDPARTYLIBS - thirdparty libaries are currently shipped with the library. 

	- \b FMILIB_FMI_STANDARD_HEADERS - Path to the FMI standard headers directory. Header for specific standards
		files are expected in subdirectories FMI1, FMI2, etc.

	- \b FMILIB_DEFAULT_BUILD_TYPE_RELEASE - Controls build-type used for Makefile generation.<br>	
	  If this option is on then 'Release' mode compile flags are used. Otherwize, 'Debug' mode flags
	  are generated into the Makefile. The option may be overwritten by explicitly setting CMAKE_BUILD_TYPE.
		
	- \b FMILIB_BUILD_WITH_STATIC_RTLIB Use static run-time libraries (/MT or /MTd code generation flags).<br>	
	  This is only used when generating Microsoft Visual Studio solutions. If the options is on then the
	  library will be built against static runtime, otherwise - dynamic runtime (/MD or /MDd). Make sure
	  the client code is using matching runtime.
		
	- \b FMILIB_BUILD_STATIC_LIB Build the library as static.
	  'fmilib' may be used for static linking.
	  
	- \b FMILIB_BUILD_SHARED_LIB Build the library as shared (dll/so/dylib).
	  '\e fmilib_shared' may be used for dynamic linking.

	- \b FMILIB_FMI_PLATFORM - FMI platform defines the subdirectory within FMU where binary is located.<br>
		The build system will automatically detect win32, win64, linux32, linux64, darwin32, darwin64.

	- \b FMILIB_BUILD_FOR_SHARED_LIBS  The static library 'fmilib' can be linked into shared libraries.<br>
	  On LINUX position independent code (-fPIC) must be used on all files to be linked into a shared 
	  library (.so file). On other systems this is not needed (either is default or relocation is done).
	  Set this option to OFF if you are building an application on Linux and use static library only.

	- \b FMILIB_ENABLE_LOG_LEVEL_DEBUG Enable log level \e 'debug'. <br>
	If the option is OFF then the debug level messages are not compiled in.

	- \b FMILIB_GENERATE_DOXYGEN_DOC Enable doxygen target.<br>
	  The build system will automatically detect if you have doxygen installed. In this case this
	  option will become available.

	- \b FMILIB_BUILD_TESTS Enable build of the tests supplied with the library.<br>
	  \c RUN_TESTS - target will be provided in Visual Studio. 'make test' will run tests on Makefile based platforms.

	- \b FMILIB_LINK_TEST_TO_SHAREDLIB Link the tests to fmilib_shared (if built) instead of static fmilib.

	- \b FMILIB_GENERATE_BUILD_STAMP Generate a build time stamp and include in into the library. <br>
	  The function \e fmilib_get_build_stamp() may be used to retrieve the time stamp.
		    \code const char* fmilib_get_build_stamp(void); \endcode

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
#include "fmilib_config.h"

#include <Common/fmi_import_context.h>
#include <FMI1/fmi1_import.h>
#endif
