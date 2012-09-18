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
#ifndef FMI2_XML_QUERY_H
#define FMI2_XML_QUERY_H

#include <jm_vector.h>
#include <jm_stack.h>
#include <FMI2/fmi2_xml_variable.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Query below has the following syntax:
  query =   elementary_query
                  | '(' query ')'
                  | query 'or' query
                  | query 'and' query
                  | 'not' query
  elementary_query =  "name" '=' <string>
                    | "quantity" '=' <string>
                    | "basetype" '=' (real| integer | enumeration |boolean |string)
                    | "type" '=' <string>
                    | "unit" '=' <string>
                    | "displayUnit" '=' <string>
                    | "fixed" '=' ("true"|"false")
                    | "hasStart" '='  ("true"|"false")
                    | "isAlias"
                    | "alias" '=' ['-']<variable name> (negative value for negated-aliases)

Example: "name='a.*' & fixed=false"
*/

#define FMI2_XML_Q_ELEMENTARY(HANDLE) \
    HANDLE(name) \
    HANDLE(unit) \

/*
    HANDLE(type) \
    HANDLE(fixed) \
    HANDLE(hasstart) \
    HANDLE(isalias) 
    HANDLE(alias)
    HANDLE(basetype) \
    HANDLE(displayunit) \
*/

typedef enum fmi2_xml_elementary_enu_t {
#define FMI2_XML_Q_ELEMENTARY_PREFIX(elem) fmi2_xml_q_elmentary_enu_##elem,
    FMI2_XML_Q_ELEMENTARY(FMI2_XML_Q_ELEMENTARY_PREFIX)
    fmi2_xml_elementary_enu_num
} fmi2_xml_elementary_enu_t;

typedef struct fmi2_xml_q_context_t fmi2_xml_q_context_t;
typedef struct fmi2_xml_q_terminal_t fmi2_xml_q_terminal_t;

typedef int (*fmi2_xml_q_scan_elementary_ft)(fmi2_xml_q_context_t*, fmi2_xml_q_terminal_t* term);

#define FMI2_XML_Q_ELEMENTARY_DECLARE_SCAN(name) int fmi2_xml_q_scan_elementary_##name(fmi2_xml_q_context_t*, fmi2_xml_q_terminal_t* term);
FMI2_XML_Q_ELEMENTARY(FMI2_XML_Q_ELEMENTARY_DECLARE_SCAN)


typedef int (*fmi2_xml_q_eval_elementary_ft)(fmi2_xml_variable_t* var, fmi2_xml_q_terminal_t* term);

#define FMI2_XML_Q_ELEMENTARY_DECLARE_EVAL(name) int fmi2_xml_q_eval_elementary_##name(fmi2_xml_variable_t* var, fmi2_xml_q_terminal_t* term);
FMI2_XML_Q_ELEMENTARY(FMI2_XML_Q_ELEMENTARY_DECLARE_EVAL)

typedef enum fmi2_xml_q_term_enu_t {
	fmi2_xml_q_term_enu_elementary,
	fmi2_xml_q_term_enu_LP,
	fmi2_xml_q_term_enu_RP,
	fmi2_xml_q_term_enu_OR,
	fmi2_xml_q_term_enu_AND,
	fmi2_xml_q_term_enu_NOT,
	fmi2_xml_q_term_enu_END,
	fmi2_xml_q_term_enu_TRUE,
	fmi2_xml_q_term_enu_FALSE
} fmi2_xml_q_terminal_enu_t;


struct fmi2_xml_q_terminal_t {
	fmi2_xml_q_terminal_enu_t kind;

	fmi2_xml_elementary_enu_t specific;

	int param_i;
	double param_d;
	void* param_p;
	char* param_str;

};

jm_vector_declare_template(fmi2_xml_q_terminal_t)

typedef jm_vector(fmi2_xml_q_terminal_t) fmi2_xml_q_term_vt;

typedef struct fmi2_xml_q_expression_t fmi2_xml_q_expression_t;

struct fmi2_xml_q_expression_t {
    jm_vector(jm_voidp) expression;

    jm_vector(jm_voidp) stack;

    fmi2_xml_q_terminal_t termFalse, termTrue;
    fmi2_xml_q_term_vt terms;
	jm_vector(char) strbuf;
};

struct fmi2_xml_q_context_t {
    jm_vector(jm_name_ID_map_t) elementary_map;

	jm_string query;

	size_t qlen;
	int curCh;

	jm_vector(char) buf;

	fmi2_xml_q_expression_t expr;
};

void fmi2_xml_q_init_context(fmi2_xml_q_context_t*, jm_callbacks* cb);
void fmi2_xml_q_free_context_data(fmi2_xml_q_context_t*);
int fmi2_xml_q_filter_variable(fmi2_xml_variable_t* var, fmi2_xml_q_expression_t* );
int fmi2_xml_q_parse_query(fmi2_xml_q_context_t* context, jm_string query);

#ifdef __cplusplus
}
#endif
#endif /* FMI2_XML_QUERY_H */
