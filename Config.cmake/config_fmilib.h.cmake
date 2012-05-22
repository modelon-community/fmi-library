#ifndef CONFIG_FMILIB_H_
#define CONFIG_FMILIB_H_

#ifndef WIN32
#cmakedefine WIN32
#endif

#cmakedefine MINGW

#ifndef CMAKE_INTDIR
#define CMAKE_INTDIR "."
#endif

/* Platform folder separator */
#define FMI_FILE_SEP "@FMI_FILE_SEP@"	

/* DLL file name suffix(.so, .dll, ...) */
#define FMI_DLL_EXT "@CMAKE_SHARED_MODULE_SUFFIX@" 

/* Folder name in which the DLL is found for this platform */
#ifdef _WIN64
#define FMI_PLATFORM "win64"
#elif defined _WIN32
#define FMI_PLATFORM "win32"
#elif (defined __LP64__ || defined __LLP64__)
#define FMI_PLATFORM "linux64"
#else
#define FMI_PLATFORM "linux32"
#endif

#define FMI_BINARIES "binaries"
#define FMI_MODEL_DESCRIPTION_XML "modelDescription.xml"

#cmakedefine FMILIB_STATIC_LIB_ONLY

#ifndef FMILIB_STATIC_LIB_ONLY
 #if defined WIN32
  #ifdef FMILIB_BUILDING_LIBRARY
    #if 0 /* defined(__GNUC__)  MSYS currently does not support visibility attribute */ 
      #define FMILIB_EXPORT __attribute__ ((dllexport))
    #else
		/* Note: actually gcc seems to also supports this syntax. */
      #define FMILIB_EXPORT __declspec(dllexport) 
    #endif
  #else
    #if 0 /* defined(__GNUC__) MSYS currently does not support visibility attribute  */
      #define FMILIB_EXPORT __attribute__ ((dllimport))
    #else
		/* Note: actually gcc seems to also supports this syntax. */
      #define FMILIB_EXPORT __declspec(dllimport)
    #endif
  #endif
  #define FMILIB_PRIVATE
 #else
  #if __GNUC__ >= 4
    #define FMILIB_EXPORT __attribute__ ((visibility ("default")))
    #define FMILIB_PRIVATE  __attribute__ ((visibility ("hidden")))
  #else
    #define FMILIB_EXPORT
    #define FMILIB_PRIVATE
  #endif
 #endif
#else
    #define FMILIB_EXPORT
    #define FMILIB_PRIVATE
#endif

#cmakedefine FMILIB_DEBUG_TRACE
#ifdef FMILIB_DEBUG_TRACE
#include <stdio.h>
#define FMILIB_TRACE(STR) printf(STR)
#else
#define FMILIB_TRACE(STR)
#endif
#cmakedefine FMILIB_GENERATE_BUILD_STAMP
#ifdef FMILIB_GENERATE_BUILD_STAMP
#ifdef __cplusplus
extern "C" {
#endif
FMILIB_EXPORT const char* fmilib_get_build_stamp(void);
#ifdef __cplusplus
}
#endif
#endif

#endif