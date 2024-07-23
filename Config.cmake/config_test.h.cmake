/*
    Copyright (C) 2012-2023 Modelon AB

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

/* Paths to test files */
#define FMU1_DLL_ME_PATH @FMU1_DLL_ME_PATH@
#define FMU1_DLL_CS_PATH @FMU1_DLL_CS_PATH@

/* Paths to test FMUs for FMI 1.0 */
#define FMU1_ME_PATH "@FMU1_ME_PATH@"

/* Paths to test FMUs for FMI 2.0 */
#define FMU2_ME_PATH "@FMU2_ME_PATH@"
#define FMU2_CS_PATH "@FMU2_CS_PATH@"

/* Paths to test FMUs for FMI 3.0 */
#define FMU3_ME_PATH "@FMU3_ME_PATH@"
#define FMU3_CS_PATH "@FMU3_CS_PATH@"
#define FMU3_SE_PATH "@FMU3_SE_PATH@"

/* Paths to temp directories */
#define FMU_UNPACK_DIR "@FMU_UNPACK_DIR@"

/* Paths to other test directories */
#define TEST_OUTPUT_FOLDER "@TEST_OUTPUT_FOLDER@"
#define FMIL_TEST_DIR "@FMIL_TEST_DIR@"
#define FMI2_TEST_XML_DIR "@FMI2_TEST_XML_DIR@"
#define FMI3_TEST_XML_DIR "@FMI3_TEST_XML_DIR@"

#define CTEST_RETURN_SUCCESS @CTEST_RETURN_SUCCESS@
#define CTEST_RETURN_FAIL @CTEST_RETURN_FAIL@

#define FMU_DUMMY_ME_MODEL_IDENTIFIER @FMU_DUMMY_ME_MODEL_IDENTIFIER@
#define FMU_DUMMY_CS_MODEL_IDENTIFIER @FMU_DUMMY_CS_MODEL_IDENTIFIER@

#cmakedefine FMILIB_LINK_TEST_TO_SHAREDLIB
#if defined(FMILIB_LINK_TEST_TO_SHAREDLIB) && defined(FMILIB_BUILDING_LIBRARY)
#undef FMILIB_BUILDING_LIBRARY
#endif

#endif
