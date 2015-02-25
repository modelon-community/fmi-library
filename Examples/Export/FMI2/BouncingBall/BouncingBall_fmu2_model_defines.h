/*
Copyright (C) 2012 Modelon AB

This program is free software: you can redistribute it and/or modify
it under the terms of the BSD style license.

the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
FMILIB_License.txt file for more details.

You should have received a copy of the FMILIB_License.txt file
along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/* This header is used to generate the FMU test DLL and in the C API test that uses the DLL */
#ifndef FMU2_MODEL_DEFINES_H_

#define BUFFER					1024
#define MAGIC_TEST_VALUE		13.0	/* A test value for some functions  */

/* BouncingBall model with redundant values */
/* ValueReferences for the variables and parameters in the model, separated by datatype */
/* States */
#define VAR_R_position			0
#define VAR_R_der_position		1
/* Real */
#define VAR_R_gravity			2
#define VAR_R_bounce_coeff		3

/* Event indicators */
#define EVENT_position			0

/* Event indicators */
#define VAR_S_LOGGER_TEST		0

/* Sizes */
#define N_STATES				2
#define N_EVENT_INDICATORS		1
#define N_REAL					4
#define N_INTEGER				4
#define N_BOOLEAN				4
#define N_STRING				4

#define N_INPUT_REAL			2 /* CS only */
#define N_INPUT_REAL_MAX_ORDER	2 /* CS only */
#define N_OUTPUT_REAL			2 /* CS only */
#define N_OUTPUT_REAL_MAX_ORDER	2 /* CS only */


#define FMI_VERSION			"2.0"

#define FMI_GUID			"f2baf4b5-6046-4c3c-a0d8-884195cf99ed"

#include <fmu2_model.h>

#endif /* End of header FMU2_MODEL_DEFINES_H_ */