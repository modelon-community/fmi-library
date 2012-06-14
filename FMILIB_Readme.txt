% FMILIB_Readme.txt
% Readme file for the FMILIB.
%

\mainpage FMI Import library (part of JModelica.org)

\version     1.0.x
\date June 2012
\section Summary
The library is intended as a foundation for applications interfacing FMUs (Functional Mockup Units)
that follow FMI Standard. See <http://functional-mockup-interface.org/> 

\section config Configuring and building
CMake (see <http://www.cmake.org/>) is used to generate the native build scripts for the library.
It is recommended to use "cmake-gui" on Windows or "ccmake" to configure the build.
The primary targets of the library build script are:
- <prefix>/include/fmilib.h \n
  The include file to be used in client applications.
- Library files under <prefix>/lib/ \n
  Static library is named 'fmilib' and shared library 'fmilib_shared'. The prefix/suffix of the library files 
  differ depending on the platform. Note that if you have configure and built both static and shared library on 
  Windows but want to link with the static library compile time define "FMILIB_BUILDING_LIBRARY" must be set.
- Doxygen generated documentation under <prefix>/doc/

The following build configuration options are provided:
- \b FMILIB_INSTALL_PREFIX - prefix prepended to install directories.\n
   This is the main install directory name. Include files will be located in the "include" subdirectory
   and library files in the "lib" subdirectory. Client applications should only include "fmilib.h"

- \b FMILIB_THIRDPARTYLIBS - thirdparty libaries are currently shipped with the library. 

- \b FMILIB_FMI_STANDARD_HEADERS - Path to the FMI standard headers directory. Header for specific standards
files are expected in subdirectories FMI1, FMI2, etc.

- \b FMILIB_DEFAULT_BUILD_TYPE_RELEASE - Controls build-type used for Makefile generation.\n
  If this option is on then 'Release' mode compile flags are used. Otherwize, 'Debug' mode flags
  are generated into the Makefile. The option may be overwritten by explicitly setting CMAKE_BUILD_TYPE.

- \b FMILIB_BUILD_WITH_STATIC_RTLIB Use static run-time libraries (/MT or /MTd code generation flags).\n
  This is only used when generating Microsoft Visual Studio solutions. If the options is on then the
  library will be built against static runtime, otherwise - dynamic runtime (/MD or /MDd). Make sure
  the client code is using matching runtime.

- \b FMILIB_BUILD_STATIC_LIB Build the library as static.
  'fmilib' may be used for static linking.
  
- \b FMILIB_BUILD_SHARED_LIB Build the library as shared (dll/so/dylib).
  '\e fmilib_shared' may be used for dynamic linking.

- \b FMILIB_FMI_PLATFORM - FMI platform defines the subdirectory within FMU where binary is located.\n
The build system will automatically detect win32, win64, linux32, linux64, darwin32, darwin64.

- \b FMILIB_BUILD_FOR_SHARED_LIBS  The static library 'fmilib' can be linked into shared libraries.\n
  On LINUX position independent code (-fPIC) must be used on all files to be linked into a shared 
  library (.so file). On other systems this is not needed (either is default or relocation is done).
  Set this option to OFF if you are building an application on Linux and use static library only.

- \b FMILIB_ENABLE_LOG_LEVEL_DEBUG Enable log level \e 'debug'. \n
If the option is OFF then the debug level messages are not compiled in.

- \b FMILIB_GENERATE_DOXYGEN_DOC Enable doxygen target.\n
  The build system will automatically detect if you have doxygen installed. In this case this
  option will become available.

- \b FMILIB_BUILD_TESTS Enable build of the tests supplied with the library.\n
  \c RUN_TESTS - target will be provided in Visual Studio. 'make test' will run tests on Makefile based platforms.
  
- \b FMILIB_BUILD_BEFORE_TESTS Force build before testing, i.e., building the FMI library becomes the first test.

- \b FMILIB_LINK_TEST_TO_SHAREDLIB Link the tests to fmilib_shared (if built) instead of static fmilib.

- \b FMILIB_GENERATE_BUILD_STAMP Generate a build time stamp and include in into the library. \n
  The function \e fmilib_get_build_stamp() may be used to retrieve the time stamp.
    \code const char* fmilib_get_build_stamp(void); \endcode

\section testing Automatic tests
The FMI library comes with a number of automatic tests. Building of the test is controlled by \a FMILIB_BUILD_TESTS 
configuration option. The test porgrams are also intended as examples of library usage.\n
The tests can be run in Visual Studio by building project \e RUN_TESTS. For Makefile based configurations (MSYS, 
Linux, Mac OSx) run "make test".\n
Output from the test programs can be found in the \e Testing folder in the build directory.

The supplied tests are:
- \b ctest_build_all \n
Build the library. This test is controlled by \a FMILIB_BUILD_BEFORE_TESTS configuration option.
- \b ctest_fmi_import_me_test \n
Load a basic model exchange FMU and simulate it. Test executable is \e fmi_import_me_test, main source file is \ref fmi_import_me_test.c.
- \b ctest_fmi_import_cs_test \n
Load a basic co-simulation FMU and simulate it. Test executable is \e fmi_import_cs_test, main source file is \ref fmi_import_cs_test.c.
- \b ctest_fmi_import_xml_test \n
Load a small model description XML file and print out detailed information on it. Test executable is \e fmi_import_xml_text,
 main source file is \ref fmi_import_xml_test.cc. 
This test depends on the success of the \e ctest_fmi_import_cs_test. 
- \b ctest_fmi_zip_unzip_test \n
Basic unzip functionality test. Test executable is \e fmi_zip_unzip_test.
- \b ctest_fmi_zip_zip_test \n
Basic zip functionality test. Test executable is \e fmi_zip_zip_test.
- \b ctest_fmi1_capi_cs_test \n
Low level test of FMI CAPI functionality for co-simulation. Test executable is \e fmi1_capi_cs_test.
- \b ctest_fmi1_capi_me_test \n
Low level test of FMI CAPI functionality for model exchange. Test executable is \e fmi1_capi_me_test.
- \b ctest_fmi1_logger_test_run \n
Run logger test generating output fmi1_logger_test_output.txt. Test executable is \e fmi1_logger_test.
- \b ctest_fmi1_logger_test_check \n
Check that the logger test output generated by \b ctest_fmi1_logger_test_run is identical with the 
reference file found in the \b Test subdirectory.

\section logging Using logs
The library is designed to send log messages to a logger callback function ::jm_logger_f provided as a part of
::jm_callbacks structure in the call to fmi_import_allocate_context().

Some more information can be found here: <https://trac.jmodelica.org/ticket/2014>

\todo Complete documentation of the logging options.
