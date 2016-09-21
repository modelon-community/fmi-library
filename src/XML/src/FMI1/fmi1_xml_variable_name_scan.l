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

%{
#include "fmi1_xml_variable_name_parser.tab.h"
#define YYSTYPE YYFMI1STYPE

#define LEX_VAL(ID_NAME) return ID_NAME;
%}

%option bison-bridge

q_name		"'"({q_char}|{s_escape})+"'"
nondigit	[_a-zA-Z]
digit		[0-9]
q_char		{nondigit}|{digit}|[^'\\]
s_escape	("\\'")|("\\\"")|("\\?")|("\\\\")|("\\a")|("\\b")|("\\f")|("\\n")|("\\r")|("\\t")|("\\v")
unsigned_integer	{digit}+

%option noyywrap
%option reentrant

%%

"der(" {LEX_VAL(DER)}
"(" {LEX_VAL('(')}
")" {LEX_VAL(')')}
", " {LEX_VAL(',')}
"," {LEX_VAL(',')}
"." {LEX_VAL('.')}
"[" {LEX_VAL('[')}
"]" {LEX_VAL(']')}
{q_name} {LEX_VAL(Q_NAME)}
{nondigit} {LEX_VAL(NONDIGIT)}
{unsigned_integer} {LEX_VAL(UNSIGNED_INTEGER)}
. {LEX_VAL(*yytext)}

%{
%}