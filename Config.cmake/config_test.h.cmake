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

#ifndef CONFIG_TEST_H
#define CONFIG_TEST_H
/* PATHs to test files */
#define FMU1_DLL_ME_PATH @FMU1_DLL_ME_PATH@ 
#define FMU1_DLL_CS_PATH @FMU1_DLL_CS_PATH@
#define COMPRESS_DUMMY_FILE_PATH_SRC "@COMPRESS_DUMMY_FILE_PATH_SRC@"
#define COMPRESS_DUMMY_FILE_PATH_DIST "@COMPRESS_DUMMY_FILE_PATH_DIST@"
#define UNCOMPRESSED_DUMMY_FILE_PATH_SRC "@UNCOMPRESSED_DUMMY_FILE_PATH_SRC@"
#define UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST "@UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST@"

#define CTEST_RETURN_SUCCESS @CTEST_RETURN_SUCCESS@
#define CTEST_RETURN_FAIL @CTEST_RETURN_FAIL@

#define FMU_DUMMY_ME_MODEL_IDENTIFIER @FMU_DUMMY_ME_MODEL_IDENTIFIER@
#define FMU_DUMMY_CS_MODEL_IDENTIFIER @FMU_DUMMY_CS_MODEL_IDENTIFIER@

#cmakedefine FMILIB_LINK_TEST_TO_SHAREDLIB
#if defined(FMILIB_LINK_TEST_TO_SHAREDLIB) && defined(FMILIB_BUILDING_LIBRARY)
#undef FMILIB_BUILDING_LIBRARY
#endif

#endif


