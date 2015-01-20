#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
	
# NOTE: CMake 2.8.6 is required since this is the version used in development.
# The script is KNOWN NOT TO WORK WITH 2.8.3 and below (ExternalProject 
# interface changes). CMake 2.8.4 and 2.8.5 are not tested.
cmake_minimum_required (VERSION 2.8.6 FATAL_ERROR)

project (FMIExamples)

# Prohibit in-source build
IF("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source build is not supported. Please, use an empty directory for building the project.")
ENDIF()

#option(FMIEXAMPLES_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
#option(FMIEXAMPLES_ENABLE_LOG_LEVEL_DEBUG "Enable log level 'debug'. If the option is off then the debug level is not compiled in." OFF)
#option(FMIEXAMPLES_PRINT_DEBUG_MESSAGES "Enable printing of status messages from the build script. Intended for debugging." OFF)


IF (NOT DEFINED ${FMILIBRARYHOME})
	
	set(FMILIBRARYHOME ${FMIExamples_SOURCE_DIR}/..)
		
	# User configuration options and parameters
	SET(FMILIB_INSTALL_PREFIX ${FMIExamples_BINARY_DIR}/../install CACHE INTERNAL "Prefix prepended to install directories")
	set(FMILIB_THIRDPARTYLIBS  ${FMILIBRARYHOME}/ThirdParty CACHE PATH "Path to the ThirdParty library dir" )
	set(FMILIB_FMI_STANDARD_HEADERS  ${FMILIB_THIRDPARTYLIBS}/FMI/default CACHE PATH "Path to the FMI standard headers dir" )

	option (FMILIB_DEFAULT_BUILD_TYPE_RELEASE "Default build-type used for Makefile generation is 'Release'. Can be overwritten by CMAKE_BUILD_TYPE command line switch." ON)
	
	if(MSVC)
		option (FMIEXAMPLES_BUILD_WITH_STATIC_RTLIB "Use static run-time libraries (/MT or /MTd linker flags)" OFF)
	endif()

	option(FMIEXAMPLES_BUILD_STATIC_LIB	 "Build the library as static." OFF)
	option(FMIEXAMPLES_BUILD_SHARED_LIB	 "Build the library as shared (dll/so/dylib)." ON)
	#option(FMILIB_INSTALL_SUBLIBS	 "Install sub-libraries (fmicapi. fmixml, jmutil, fmiimport, fmizip)" OFF)

	IF(NOT (WIN32 OR CYGWIN OR APPLE))
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

	#option (FMILIB_GENERATE_DOXYGEN_DOC "Generate doxygen doc target" ON)

	#option(FMILIB_GENERATE_BUILD_STAMP "Generate a build time stamp and include in into the library" OFF)
	option(FMIEXAMPLES_ENABLE_LOG_LEVEL_DEBUG "Enable log level 'debug'. If the option is of then the debug level is not compiled in." OFF)
	option(FMIEXAMPLES_PRINT_DEBUG_MESSAGES "Enable printing of status messages from the build script. Intended for debugging." OFF)
	mark_as_advanced(FMIEXAMPLES_PRINT_DEBUG_MESSAGES FMILIB_DEBUG_TRACE)

	if(FMIEXAMPLES_DEFAULT_BUILD_TYPE_RELEASE)
		set (FMIEXAMPLES_DEFAULT_BUILD_TYPE "Release")
	else()
		set (FMIEXAMPLES_DEFAULT_BUILD_TYPE "Debug")
	endif()

	IF(NOT CMAKE_BUILD_TYPE)
		SET(CMAKE_BUILD_TYPE ${FMILIB_DEFAULT_BUILD_TYPE})
	ENDIF(NOT CMAKE_BUILD_TYPE)

	SET(CMAKE_INSTALL_PREFIX ${FMILIB_INSTALL_PREFIX} CACHE INTERNAL "Prefix prepended to install directories" FORCE)

	IF(NOT CMAKE_CFG_INTDIR)
		SET(CMAKE_CFG_INTDIR ${FMILIB_DEFAULT_BUILD_TYPE})
	ENDIF(NOT CMAKE_CFG_INTDIR)

	if(MSVC)
		# With C89 we're forced to use non-secure functions
		ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
	
		# Set the run-time library flag as controlled by the FMILIB_BUILD_WITH_STATIC_RTLIB option
		if(FMIEXAMPLES_BUILD_WITH_STATIC_RTLIB)
			set(BUILD_WITH_STATIC_CRT YES)
			set(replace_from "/MD")
			set(replace_to "/MT")
		else()
			set(BUILD_WITH_STATIC_CRT NO)
			set(replace_from "/MT")
			set(replace_to "/MD")
		endif(FMIEXAMPLES_BUILD_WITH_STATIC_RTLIB)

		foreach(flag_var
			CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
			CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
			if(${${flag_var}} MATCHES "${replace_from}")
				string(REPLACE "${replace_from}" "${replace_to}" tmp "${${flag_var}}")
				set(${flag_var} ${tmp} CACHE STRING "compiler flags" FORCE)
			endif()
		endforeach(flag_var)	
	endif(MSVC)

	IF(CMAKE_COMPILER_IS_GNUCC)
		SET(SAVED_C_DEFAULT_FLAGS "${CMAKE_C_FLAGS}")
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall -Wno-unused-function")
		if(NOT MSYS)
			include(CheckCCompilerFlag)
			CHECK_C_COMPILER_FLAG("-fvisibility=hidden" SUPPORT_VISIBILITY)
			if(SUPPORT_VISIBILITY)
				SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
			endif()
		endif()
	ENDIF(CMAKE_COMPILER_IS_GNUCC)

	if(FMIEXAMPLES_BUILD_FOR_SHARED_LIBS) 
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
		SET(WITH_PIC ON) # this is used in MERGE_LIBRARIES below
	endif()


	#Cmake variables set in config files.
	if(WIN32)
		set(FMI_FILE_SEP "\\\\")
	else(WIN32)
		set(FMI_FILE_SEP "/")
	endif(WIN32)

	set(FMIEXAMPLES_FMI_PLATFORM_DOC "FMI platform defines the subdirectory within FMU where binary is located")
	set(FMIEXAMPLES_FMI_PLATFORM ”unknown” CACHE STRING ${FMIEXAMPLES_FMI_PLATFORM_DOC})
	if(FMIEXAMPLES_FMI_PLATFORM MATCHES ”unknown”)
		if(CMAKE_HOST_WIN32) #Set to true when the host system is Windows and on cygwin.
			if(CMAKE_CL_64)
				set(FMI_PLATFORM win64)
			else(CMAKE_CL_64)
				set(FMI_PLATFORM win32)
			endif(CMAKE_CL_64)
		elseif(CMAKE_HOST_APPLE) #Set to true when the host system is Apple OSX. 
			if(CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(FMI_PLATFORM darwin32)
			else(CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(FMI_PLATFORM darwin64)
			endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		elseif(CMAKE_HOST_UNIX) #Set to true when the host system is UNIX or UNIX like (i.e. APPLE and CYGWIN). 
			if(CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(FMI_PLATFORM linux32)
			else(CMAKE_SIZEOF_VOID_P EQUAL 4)
				set(FMI_PLATFORM linux64)
			endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		else(CMAKE_HOST_WIN32)
			message(SEND_ERROR "Target platform is not supported by the tests")
			set(FMI_PLATFORM not_supported)
		endif()
	
		set(FMIEXAMPLES_FMI_PLATFORM ${FMI_PLATFORM} CACHE STRING ${FMIEXAMPLES_FMI_PLATFORM_DOC} FORCE)
	else()
		set(FMI_PLATFORM ${FMIEXAMPLES_FMI_PLATFORM})
	endif()

	
#	install(DIRECTORY 
#		${FMILIB_FMI_STANDARD_HEADERS}/FMI1
#		${FMILIB_FMI_STANDARD_HEADERS}/FMI2
#		DESTINATION include
#		FILES_MATCHING PATTERN "*.h")

	# this is used when searching for include files e.g. using the FIND_PATH() command.
	#MESSAGE( STATUS "FMILIB_FMI_STANDARD_HEADERS: " ${FMILIB_FMI_STANDARD_HEADERS} )
	
	include_directories("${FMILIB_FMI_STANDARD_HEADERS}")
		
	function(to_native_c_path path native_c_path)
		if (WIN32)
			STRING(REPLACE "/" "\\\\" tmp "${path}")
		else()
			set(tmp "${path}")
		endif()
		set (${native_c_path} ${tmp} PARENT_SCOPE)
	endfunction()

	#Move files and compress them to an FMU
	function(compress_fmu OUTPUT_FOLDER_T MODEL_IDENTIFIER_T  TARGET_NAME_T XML_PATH_T SHARED_LIBRARY_PATH_T)
		set(FMU_OUTPUT_FOLDER_T ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T})
		set(FMU_OUTPUT_SHARED_LIBRARY_PATH_T ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX})

		#Must create the FMU directory in a separate command..
		if(NOT EXISTS ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM})
			file(MAKE_DIRECTORY ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM})		
		endif()

		file(TO_NATIVE_PATH binaries/${FMI_PLATFORM}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX} FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T)
	
		#Move files to the FMU directories and compress 
		ADD_CUSTOM_COMMAND(
		OUTPUT ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu
		#DEPENDS "${XML_PATH_T}" ${TARGET_NAME_T} compress_test_fmu_zip	   
		DEPENDS "${XML_PATH_T}" ${TARGET_NAME_T} ${ZIP_COMMAND}	   
		COMMAND "${CMAKE_COMMAND}" -E remove -f "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu"
		COMMAND "${CMAKE_COMMAND}" -E copy "${XML_PATH_T}" "${FMU_OUTPUT_FOLDER_T}/modelDescription.xml"
		COMMAND "${CMAKE_COMMAND}" -E copy "${SHARED_LIBRARY_PATH_T}" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_T}"
		#COMMAND "${COMPRESS_EXECUTABLE}" "${MODEL_IDENTIFIER_T}.fmu" "modelDescription.xml" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T}" WORKING_DIRECTORY "${FMU_OUTPUT_FOLDER_T}"
		#COMMAND ${ZIP_COMMAND} "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.zip" "${FMU_OUTPUT_FOLDER_T}/modelDescription.xml" "${FMU_OUTPUT_FOLDER_T}"
		COMMAND ${ZIP_COMMAND} "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.zip" "${FMU_OUTPUT_FOLDER_T}/modelDescription.xml" "${FMU_OUTPUT_FOLDER_T}/*"
		COMMAND "${CMAKE_COMMAND}" -E rename "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.zip" "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu"
		)
	
		get_target_property(DLL_SOURCES ${TARGET_NAME_T} SOURCES)
  
		set(tname ${MODEL_IDENTIFIER_T}_FMU)
		add_custom_target( ${tname} ALL
			DEPENDS ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu 
			SOURCES "${XML_PATH_T}" ${DLL_SOURCES})
#		set_target_properties( ${tname} ${TARGET_NAME_T}
 #                       PROPERTIES FOLDER "TestFMUs")
	
	endfunction(compress_fmu)

ELSE()
	
	set(FMIEXAMPLES_DEFAULT_BUILD_TYPE $(FMILIB_DEFAULT_BUILD_TYPE))
	
ENDIF()


#set(FMIEXAMPLESHOME ${FMILIBRARYHOME}/Examples)
#set(FMILibrary_SOURCE_DIR ${FMILIBRARYHOME})
#set(FMIExamples_SOURCE_DIR ${FMIEXAMPLESHOME})
#set(FMIEXAMPLESBUILD ${FMIExamples_BINARY_DIR})

set(FMUEXPORTDIR ${FMIExamples_SOURCE_DIR}/Export)

set(EXPORT_OUTPUT_FOLDER "${FMIExamples_BINARY_DIR}/ExampleExport")

FIND_PROGRAM(ZIP_EXECUTABLE wzzip PATHS "$ENV{ProgramFiles}/WinZip")
IF(ZIP_EXECUTABLE)
  SET(ZIP_COMMAND "\"${ZIP_EXECUTABLE}\" -P \"<ARCHIVE>\" @<FILELIST>")
ENDIF(ZIP_EXECUTABLE)

FIND_PROGRAM(ZIP_EXECUTABLE 7z PATHS "$ENV{ProgramFiles}/7-Zip") 
IF(ZIP_EXECUTABLE)
  SET(ZIP_COMMAND ${ZIP_EXECUTABLE} a -tzip)
ENDIF(ZIP_EXECUTABLE)

#find_package(Java)
#
#execute_process(
#    COMMAND
#        "${Java_JAR_EXECUTABLE}" "cfM" "archive.zip" 
#        "-C" "${CMAKE_CURRENT_SOURCE_DIR}" "testfile.txt" 
#        "-C" "${CMAKE_CURRENT_SOURCE_DIR}" "testdir"
#    RESULT_VARIABLE _result
#)


#Defines for the BouncingBall FMU
set(FMU_BB1_ME_MODEL_IDENTIFIER BouncingBall_FMI1_ME) #This must be the same as in the xml-file
set(FMU_BB1_CS_MODEL_IDENTIFIER BouncingBall_FMI1_CS) #This must be the same as in the xml-file
set(FMU_BB2_MODEL_IDENTIFIER BouncingBall_FMI2) #This must be the same as in the xml-file

set(FMU_BB1_FOLDER ${FMUEXPORTDIR}/FMI1/BouncingBall)
set(FMU_BB2_FOLDER ${FMUEXPORTDIR}/FMI2/BouncingBall)

include_directories("${FMU_BB1_FOLDER}")
include_directories("${FMU_BB2_FOLDER}")

set(FMU_BB1_ME_SOURCE
  ${FMU_BB1_FOLDER}/fmu1_model_me.c
)
set(FMU_BB1_CS_SOURCE
  ${FMU_BB1_FOLDER}/fmu1_model_cs.c
)
set(FMU_BB1_HEADERS
  ${FMU_BB1_FOLDER}/fmu1_model.h
  ${FMU_BB1_FOLDER}/fmu1_model_defines.h
)

set(FMU_BB2_SOURCE
  ${FMU_BB2_FOLDER}/fmu2_model_cs_me.c
)
set(FMU_BB2_HEADERS
  ${FMU_BB2_FOLDER}/fmu2_model.h
  ${FMU_BB2_FOLDER}/fmu2_model_defines.h
)


add_library(BouncingBall_fmu1_dll_me SHARED ${FMU_BB1_ME_SOURCE} ${FMU_BB1_HEADERS})
add_library(BouncingBall_fmu1_dll_cs SHARED ${FMU_BB1_CS_SOURCE} ${FMU_BB1_HEADERS})
add_library(BouncingBall_fmu2_dll SHARED ${FMU_BB2_SOURCE} ${FMU_BB2_HEADERS})


set(XML_BB1_ME_PATH ${FMU_BB1_FOLDER}/modelDescription_me.xml)
set(XML_BB1_CS_PATH ${FMU_BB1_FOLDER}/modelDescription_cs.xml)
set(XML_BB2_PATH ${FMU_BB2_FOLDER}/modelDescription_cs_me.xml)

set(SHARED_LIBRARY_BB1_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_BB1_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_BB2_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu2_dll${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create BouncingBall FMU 1.0 ME/CS Model and generate library path to be used in test config
#to_native_c_path("\"${SHARED_LIBRARY_BB1_ME_PATH}\"" DLL_OUTPUT_PATH_BB_ME_DEFINE)
#to_native_c_path("\"${SHARED_LIBRARY_BB1_CS_PATH}\"" DLL_OUTPUT_PATH_BB_CS_DEFINE)
#to_native_c_path("\"${SHARED_LIBRARY_BB2_PATH}\"" DLL_OUTPUT_PATH_BB_CS_DEFINE)

to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				BB_FMU1_DLL_ME_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				BB_FMU1_DLL_CS_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}BouncingBall_fmu2_dll${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				BB_FMU2_DLL_PATH)
				
				
compress_fmu("${EXPORT_OUTPUT_FOLDER}" "${FMU_BB1_ME_MODEL_IDENTIFIER}" "BouncingBall_fmu1_dll_me" "${XML_BB1_ME_PATH}" "${SHARED_LIBRARY_BB1_ME_PATH}")
compress_fmu("${EXPORT_OUTPUT_FOLDER}" "${FMU_BB1_CS_MODEL_IDENTIFIER}" "BouncingBall_fmu1_dll_cs" "${XML_BB1_CS_PATH}" "${SHARED_LIBRARY_BB1_CS_PATH}")
compress_fmu("${EXPORT_OUTPUT_FOLDER}" "${FMU_BB2_MODEL_IDENTIFIER}" "BouncingBall_fmu2_dll" "${XML_BB2_PATH}" "${SHARED_LIBRARY_BB2_PATH}")


file(MAKE_DIRECTORY "${FMILIB_INSTALL_PREFIX}/ExampleFMUs")

install(FILES 
			"${EXPORT_OUTPUT_FOLDER}/${FMU_BB1_ME_MODEL_IDENTIFIER}.fmu"
			"${EXPORT_OUTPUT_FOLDER}/${FMU_BB1_CS_MODEL_IDENTIFIER}.fmu"
			"${EXPORT_OUTPUT_FOLDER}/${FMU_BB2_MODEL_IDENTIFIER}.fmu"
		DESTINATION ExampleFMUs)
		