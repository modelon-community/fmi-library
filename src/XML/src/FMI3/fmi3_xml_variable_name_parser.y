/*
    Copyright (C) 2023 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

/* http://www.gnu.org/software/bison/manual/bison.html#Decl-Summary */
%{

/*
#define YYDEBUG 1
int yydebug = 1;
*/

#include <stdio.h>
#include "fmi3_xml_variable_name_parser.tab.h"
#include "fmi3_xml_variable_name_lex.h"

#define MODULE "FMI3XML"
#include <JM/jm_portability.h>

void yyfmi3error(yyscan_t* scanner, jm_callbacks* cb, char* name, char const *msg)
{
	jm_log_error(cb, MODULE, "Invalid structured ScalarVariable name \"%s\": %s", name, msg);
}

%}

%require "2.7"
%expect 0 //Declare the expected number of shift-reduce conflicts

%error-verbose

%define api.pure full

%parse-param {void* scanner}
%parse-param {void* cb}
%parse-param {char* name}
%lex-param {yyscan_t scanner}

%language "C"

%token DER UNSIGNED_INTEGER NONDIGIT Q_NAME

%start name
%%

name:
	identifier_list
	| DER identifier_list ')'
	| DER identifier_list ',' UNSIGNED_INTEGER ')'
	;

identifier_list:
	identifier
	| identifier_list '.' identifier
	;

identifier:
	bname
	| bname '[' arrayIndices ']'
    ;

bname:
	NONDIGIT nondigit_or_digit
	| Q_NAME
	;

nondigit_or_digit:
	/* empty */
	| UNSIGNED_INTEGER nondigit_or_digit
	| NONDIGIT nondigit_or_digit
	;

arrayIndices:
	UNSIGNED_INTEGER
	| arrayIndices ',' UNSIGNED_INTEGER
	;

%%
