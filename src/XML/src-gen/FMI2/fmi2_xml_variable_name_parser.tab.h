/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YYFMI2_FMI2_XML_VARIABLE_NAME_PARSER_TAB_H_INCLUDED
# define YY_YYFMI2_FMI2_XML_VARIABLE_NAME_PARSER_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYFMI2DEBUG
# if defined YYDEBUG
#  if YYDEBUG
#   define YYFMI2DEBUG 1
#  else
#   define YYFMI2DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define YYFMI2DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined YYFMI2DEBUG */
#if YYFMI2DEBUG
extern int yyfmi2debug;
#endif

/* Tokens.  */
#ifndef YYFMI2TOKENTYPE
# define YYFMI2TOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yyfmi2tokentype {
     DER = 258,
     UNSIGNED_INTEGER = 259,
     NONDIGIT = 260,
     Q_NAME = 261
   };
#endif


#if ! defined YYFMI2STYPE && ! defined YYFMI2STYPE_IS_DECLARED
typedef int YYFMI2STYPE;
# define YYFMI2STYPE_IS_TRIVIAL 1
# define yyfmi2stype YYFMI2STYPE /* obsolescent; will be withdrawn */
# define YYFMI2STYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyfmi2parse (void *YYPARSE_PARAM);
#else
int yyfmi2parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyfmi2parse (void* scanner, void* cb, char* name);
#else
int yyfmi2parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YYFMI2_FMI2_XML_VARIABLE_NAME_PARSER_TAB_H_INCLUDED  */
