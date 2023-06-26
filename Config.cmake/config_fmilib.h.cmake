/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef CONFIG_FMILIB_H_
#define CONFIG_FMILIB_H_

/** \file fmilib_config.h
    \brief Library configuration file generated by the build system. 
*/

#ifndef WIN32
#cmakedefine WIN32
#ifdef WIN32
/** \def WIN32
    \brief WIN32 is used to identify Windows OS (set on both Win32 and Win64) 
*/
#endif
#endif

#cmakedefine MINGW
#ifdef MINGW
/** \def MINGW
    \brief MINGW is used to identify MSYS/MinGW environment 
*/
#endif

#ifndef CMAKE_INTDIR
#define CMAKE_INTDIR "."
/** \def CMAKE_INTDIR
    \brief The macro #CMAKE_INTDIR gives intermediate directory for the current configuration (Debug/Release). 

It is redefined to "." for Makefile based build environments. Otherwise is set by MSVC. */
#endif

/** \brief Platform folder separator */
#define FMI_FILE_SEP "@FMI_FILE_SEP@"    

/** \brief DLL file name suffix(.so, .dll, ...) */
#define FMI_DLL_EXT "@CMAKE_SHARED_LIBRARY_SUFFIX@" 

/** \brief Folder name inside FMU in which the DLL is found for this platform for FMI 1 */
#define FMI1_PLATFORM "@FMI1_PLATFORM@"

/** \brief Folder name inside FMU in which the DLL is found for this platform for FMI 2 */
#define FMI2_PLATFORM "@FMI2_PLATFORM@"

/** \brief Folder name inside FMU in which the DLL is found for this platform for FMI 3 */
#define FMI3_PLATFORM "@FMI3_PLATFORM@"

/** \brief Folder name inside FMU where binaries are */
#define FMI_BINARIES "binaries"
/** \brief Model description XML file name */
#define FMI_MODEL_DESCRIPTION_XML "modelDescription.xml"

#cmakedefine FMILIB_STATIC_LIB_ONLY
#ifdef FMILIB_STATIC_LIB_ONLY
/** 
\def FMILIB_STATIC_LIB_ONLY
\brief The macro #FMILIB_STATIC_LIB_ONLY is defined if no fmilib_shared is build. */
#endif

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
  #if __GNUC__ >= 4
    #define FMILIB_EXPORT __attribute__ ((visibility ("default")))
    #define FMILIB_PRIVATE  __attribute__ ((visibility ("hidden")))
  #else
    #define FMILIB_EXPORT
    #define FMILIB_PRIVATE
  #endif
#endif

/** \def FMILIB_EXPORT
    \brief Used to declare the public API of the library needed for dynamic linking. 
    \def FMILIB_PRIVATE
    \brief Intended for declaration of the private library functions.
*/

#cmakedefine FMILIB_ENABLE_LOG_LEVEL_DEBUG
#ifndef FMILIB_ENABLE_LOG_LEVEL_DEBUG
/* Just for doxygen */
#define FMILIB_ENABLE_LOG_LEVEL_DEBUG
#undef FMILIB_ENABLE_LOG_LEVEL_DEBUG
#endif
/** 
\def FMILIB_ENABLE_LOG_LEVEL_DEBUG
\brief Activates debug level log messages. If not defined the debug messages are compiled out. 
*/

#if defined _MSC_VER
    #define FMILIB_SIZET_FORMAT "%Iu"
#else 
    #define FMILIB_SIZET_FORMAT "%zu"
#endif

#cmakedefine FMILIB_GENERATE_BUILD_STAMP
#ifndef FMILIB_GENERATE_BUILD_STAMP
/* Just for Doxygen */
#define FMILIB_GENERATE_BUILD_STAMP
#undef FMILIB_GENERATE_BUILD_STAMP
#endif
/** 
\def FMILIB_GENERATE_BUILD_STAMP
\brief Generate build-stamp file and enable fmilib_get_build_stamp(). 
*/

#ifdef FMILIB_GENERATE_BUILD_STAMP
#ifdef __cplusplus
extern "C" {
#endif

/** \brief Get the library build stamp. 
    @return A string giving SVN revision and build time for the library.

    The function is only active if the library was configured with #FMILIB_GENERATE_BUILD_STAMP ON
*/
FMILIB_EXPORT const char* fmilib_get_build_stamp(void);
#ifdef __cplusplus
}
#endif
#endif

#cmakedefine HAVE_VA_COPY
#cmakedefine HAVE___VA_COPY

#cmakedefine HAVE_VSNPRINTF
#ifdef HAVE_VSNPRINTF
#define jm_rpl_vsnprintf vsnprintf
#endif

#endif
