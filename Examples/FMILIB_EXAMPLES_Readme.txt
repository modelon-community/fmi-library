% FMILIB_Examples_Readme.txt
% Readme file for the FMI Library Examples (part of JModelica.org)
%  
% Content of this file is used verbatim in doxygen generated documentation.
% The documentation is available online in HTML and PDF form at
% http://www.fmi-library.org
%

\mainpage FMI Library Examples

\version     2.0.2
\date        29 January 2015 
\section Summary
FMI Library Examples is intended as an introduction to handcoding FMUs (Functional Mockup Units) that follow FMI Standard. This version of the library supports FMI 1.0 and FMI2.0.
See <http://www.fmi-standard.org/> 

The test code provided with the library examples can serve as starting point for the 
development of custom FMUs.

\section config Configuring and building
CMake (see <http://www.cmake.org/>) is used to generate the native build scripts
for the library. It is recommended to use "cmake-gui" on Windows or 
"ccmake <FMIL source dir>" to configure the build. All the required third party
libraries are included in the FMI Library distribution.

CMake 2.8.6 is required since this is the version used in development both on 
Windows and Linux. The build script is KNOWN NOT TO WORK WITH CMake 2.8.3 and 
below (due to ExternalProject interface changes). CMake 2.8.4 and 2.8.5 are not 
tested.

The examples are built with the FMI Library by default, but may also be built independently. When building standalone, please make sure to provide the FMI Standard Headers.

To build from a terminal command line on Linux or Mac with default settings use:
\code
	mkdir build-fmilexamples; cd build-fmilexamples
	cmake -DFMILIB_INSTALL_PREFIX=<prefix> -DFMILIB_FMI_STANDARD_HEADERS=../ThirdParty/FMI/default <path to FMILExamples source>
	make install
\endcode
To build in MSYS terminal with g++/gcc on Windows:
\code
	mkdir build-fmilexamples; cd build-fmilexamples
	cmake -DFMILIB_INSTALL_PREFIX=<prefix> -DFMILIB_FMI_STANDARD_HEADERS=../ThirdParty/FMI/default -G "MSYS Makefiles" <path to FMILExamples source>
	make install test
\endcode
To build from command line with Microsoft Visual Studio compilers on Windows:
\code
	mkdir build-fmil; cd build-fmil
	cmake -DFMILIB_INSTALL_PREFIX=<prefix> -DFMILIB_FMI_STANDARD_HEADERS=../ThirdParty/FMI/default -G "Visual Studio 10" <path to FMIL source>
	cmake --build . --config MinSizeRel --target install
\endcode

  
The following build configuration options are provided:
- \b FMILIB_INSTALL_PREFIX - prefix prepended to install directories.\n
   Default: "../install" \n
   This is the main install directory name. Include files will be located in 
   the "include" subdirectory and library files in the "lib" subdirectory. 
   Client applications should only include "fmilib.h"

- \b FMILIB_FMI_STANDARD_HEADERS - Path to the FMI standard headers 
  directory. Header for specific standards files are expected in 
  subdirectories FMI1, FMI2, etc.\n
  Default: "ThirdParty/FMI/default"

- \b FMILIB_DEFAULT_BUILD_TYPE_RELEASE - Controls build-type used for 
  Makefile generation.\n  
  Default: ON\n
  If this option is on then 'Release' mode compile flags are used. Otherwize, 
  'Debug' mode flags are generated into the Makefile. The option may be 
  overwritten by explicitly setting CMAKE_BUILD_TYPE.

- \b FMILIB_BUILD_WITH_STATIC_RTLIB Use static run-time libraries (/MT or 
  /MTd code generation flags).\n
  Default: OFF\n
  This is only used when generating Microsoft Visual Studio solutions. If the 
  options is on then the library will be built against static runtime, 
  otherwise - dynamic runtime (/MD or /MDd). Make sure the client code is 
  using matching runtime.

- \b FMILIB_FMI_PLATFORM - FMI platform defines the subdirectory within FMU 
  where binary is located.\n
  The build system will automatically detect win32, win64, linux32, linux64, 
  darwin32, darwin64.


\section code Code Overview
The FMI Library Examples contains example code to compile FMUs according to FMI Standard 
1.0 and 2.0. The sources are diveded by the implemented standard and by example type.

For FMI 1.0 the code produces separate ModelExchange and CoSimulation FMUs, for FMI 2.0 this 
is according to standard implemented in one FMU. Therefore the code is setup in sections 
providing function definition for CS, ME and common definitions, which are used for both. 

The file "fmuX_model.c" contains all functional code (equations for calculations, getting and 
setting values,...) required for both implementations. 
The files "fmu1_model_cs.c" and "fmu1_model_me.c" contain the indirections to match all function 
declarations required by the FMI standard. For FMI 2.0 this can be applied in one file 
"fmu2_model_cs_me.c".

