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
#ifndef FMU_VAL2_MODEL_DEFINES_H_


#define STRINGIFY(a)			#a
#define STRINGIFY2(a)			STRINGIFY(a)
#define MODEL_IDENTIFIER_STR	STRINGIFY2(MODEL_IDENTIFIER)

#define BUFFER					1024
#define MAGIC_TEST_VALUE		13.0	/* A test value for some functions  */

/* Values feedthrough for all dataypes */
/* ValueReferences for the variables and parameters in the model */
/*Dummy state*/
#define VAR_R_dummy				0
/* Real */
#define VAR_R_real_input		1
#define VAR_R_real_output		2
/* Booelan */
#define VAR_B_boolean_input		0
#define VAR_B_boolean_output	1
/* Integer */
#define VAR_I_integer_input		0
#define VAR_I_integer_output	1
/* String */
#define VAR_S_string_input		0
#define VAR_S_string_output		1

/* Event indicators */
#define VAR_S_LOGGER_TEST		0


/* Sizes */
#define N_STATES				1 /*dummy value 1*/
#define N_EVENT_INDICATORS		1 /*dummy value 1*/
#define N_REAL					3
#define N_INTEGER				2
#define N_BOOLEAN				2
#define N_STRING				2

#define N_INPUT_REAL			1 /* CS only */
#define N_INPUT_REAL_MAX_ORDER	2 /* CS only */
#define N_OUTPUT_REAL			1 /* CS only */
#define N_OUTPUT_REAL_MAX_ORDER	2 /* CS only */

#define FMI_VERSION			"2.0"

#define FMI_GUID			"283b6866-f3cb-4483-a80c-d255238c0c39"

#include "fmu2_model.h"

#endif /* End of header FMU1_MODEL_DEFINES_H_ */