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
#include <stdio.h>
#include <ctype.h>

#include <jm_vector.h>
#include <jm_stack.h>
#include <jm_wc_match.h>
#include "fmi1_xml_query.h"


jm_name_ID_map_t fmi1_xml_q_elementary_map[fmi1_xml_elementary_enu_num+1] =
{
	#define FMI1_XML_Q_ELEMENTARY_MAP(name) {#name , fmi1_xml_q_elmentary_enu_##name},
    FMI1_XML_Q_ELEMENTARY(FMI1_XML_Q_ELEMENTARY_MAP)
	{0,0}
};

fmi1_xml_q_scan_elementary_ft fmi1_xml_q_scan_elementary_handles[fmi1_xml_elementary_enu_num + 1] = {
	#define FMI1_XML_Q_ELEMENTARY_SCAN(name) fmi1_xml_q_scan_elementary_##name,
    FMI1_XML_Q_ELEMENTARY(FMI1_XML_Q_ELEMENTARY_SCAN)
	0
};

fmi1_xml_q_eval_elementary_ft fmi1_xml_q_eval_elementary_handles[fmi1_xml_elementary_enu_num + 1] = {
	#define FMI1_XML_Q_ELEMENTARY_EVAL(name) fmi1_xml_q_eval_elementary_##name,
    FMI1_XML_Q_ELEMENTARY(FMI1_XML_Q_ELEMENTARY_EVAL)
	0
};

jm_name_ID_map_t fmi1_xml_q_op_map[] = {
	{"or", fmi1_xml_q_term_enu_OR},
	{"and", fmi1_xml_q_term_enu_AND},
	{"not", fmi1_xml_q_term_enu_NOT},
	{"&&", fmi1_xml_q_term_enu_AND},
	{"||", fmi1_xml_q_term_enu_OR},
	{"!", fmi1_xml_q_term_enu_NOT},
	{0,-1}
};

static void fmi1_xml_q_skip_space(jm_string* cur) {
    jm_string curChP = *cur;
    char curCh;
    if(!curChP) return;
    curCh = *curChP;
    while(curCh || (curCh == ' ') || (curCh == '\t')) {
        curChP++; curCh = *curChP;
    }
    *cur = curChP;
}

int fmi1_xml_q_scan_string(fmi1_xml_q_context_t* context, char** param_str) {
	fmi1_xml_q_expression_t* expr = &context->expr;
	char* dest; 
	jm_string cur = context->query + context->curCh;
    char ch = *cur;
    char strterm ;
    size_t strlen = 0; 

	if((ch == '\'') || (ch == '"')) /* either ' or " can be used as string terminator */
		strterm = ch;
    else 
		return -1;
    do {
        ch = cur[strlen+1];
        dest = jm_vector_push_back(char)(&expr->strbuf, ch);
		assert(dest);
        strlen++;
    } while((ch != strterm) && ch);
    if(!ch) return -1; /* string is not terminated */
	*dest = 0; /* put terminating 0*/
	strlen--; /* last zero is not a part of the string */
	*param_str = dest - strlen;
    return strlen;
}


int fmi1_xml_q_scan_elementary_name(fmi1_xml_q_context_t* context, fmi1_xml_q_terminal_t* term) {
	jm_string startCh = &context->query[context->curCh];
	jm_string curCh = startCh;
	size_t len;

	/* expecting: [<spaces>]'='[<spaces>]<string> */

	fmi1_xml_q_skip_space(&curCh);
	if(*curCh != '=') 
		return (int)(startCh - curCh);
	curCh++;
	fmi1_xml_q_skip_space(&curCh);

	len = (int)(curCh - startCh);

	context->curCh += len;
	startCh += len;

	if( fmi1_xml_q_scan_string(context, &term->param_str)  < 0) 
		return (int)(startCh - curCh);
	
	/*	treat as regexp - > skip for now
	if(term->param_str[0] == '^') {		

	}
	else  */
	{		
		term->param_i = strlen(term->param_str);
		if( (strchr ( term->param_str, '*') != 0) || (strchr ( term->param_str, '?') != 0) ){
			/* treat as wildcard */
			term->param_i *= -1;
		}
	}
	return (int)(curCh - startCh);
}

int fmi1_xml_q_eval_elementary_name(fmi1_xml_variable_t* var, fmi1_xml_q_terminal_t* term) {
	assert(term->specific == fmi1_xml_q_elmentary_enu_name);

	if(term->param_i < 0) {
		return jm_wc_match(term->param_str, fmi1_xml_get_variable_name(var));
	}
	else
		return (strncmp(term->param_str, fmi1_xml_get_variable_name(var), term->param_i) == 0);
	return 0;
}

int fmi1_xml_q_scan_elementary_unit(fmi1_xml_q_context_t* context, fmi1_xml_q_terminal_t* term) {
	jm_string startCh = &context->query[context->curCh];
	jm_string curCh = startCh;
	size_t len;

	/* expecting: [<spaces>]'='[<spaces>]<string> */

	fmi1_xml_q_skip_space(&curCh);
	if(*curCh != '=') 
		return (int)(startCh - curCh);
	curCh++;
	fmi1_xml_q_skip_space(&curCh);

	len = (int)(curCh - startCh);

	context->curCh += len;
	startCh += len;

	if( fmi1_xml_q_scan_string(context, &term->param_str)  < 0) 
		return (int)(startCh - curCh);
	return (int)(curCh - startCh);
}

int fmi1_xml_q_eval_elementary_unit(fmi1_xml_variable_t* var, fmi1_xml_q_terminal_t* term) {
	return 0;
}

int fmi1_xml_q_get_number(fmi1_xml_q_context_t* context, char* cur, double* val, char* buf) {
	int len;
	if(sscanf(cur, "%lg%s", val, buf) != 2) return 0;
	len = strlen(cur) - strlen(buf);
	return len;
}

int fmi1_xml_q_get_keyword(fmi1_xml_q_context_t* context, char* cur, size_t* len, char* buf) {
    char ch = *cur;
    size_t i = 0, id;
    *len = 0;
	if( (ch == '|') || (ch == '&') ) {		
		if(ch ==  cur[i+1]) {
			buf[i++] = ch;
			buf[i++] = ch;
		}
	}
	else {
		while(isalpha(ch)) {
			buf[i++] = tolower(ch);
			ch = cur[i];
		}
	}
	
    if(!i) return -1;

	{
		jm_name_ID_map_t key;
		jm_name_ID_map_t* map;
		key.name = buf;
		map = jm_vector_bsearch(jm_name_ID_map_t)(&context->elementary_map, &key,jm_compare_name);
		if(!map) return -1;
		id = map->ID;
	}
    *len = i;
    return id;
}


/* if we ever get to regex then this might be a good function */ 
int pattern2regexp(const char* pattern, jm_vector(char)* re) {
    size_t plen = strlen(pattern), i;
    if(jm_vector_reserve_char(re, plen * 2 + 3) < plen) return -1;
    jm_vector_resize_char(re, 0);
    jm_vector_push_back_char(re, '^');
    for(i=0; i < plen; i++) {
        char cur = pattern[i];
        switch(cur) {
        case '*':
            jm_vector_push_back_char(re, '.');
            jm_vector_push_back_char(re,'*');
            break;
        case '?':
            jm_vector_push_back_char(re,'.');
            break;
        default:
            jm_vector_push_back_char(re,'\\');
            jm_vector_push_back_char(re,cur);
        }
    }
    jm_vector_push_back_char(re, '$');
    jm_vector_push_back_char(re, 0);
    return 0;
}

int fmi1_xml_evaluate_terminal(fmi1_xml_variable_t* var, fmi1_xml_q_terminal_t* term) {
	return fmi1_xml_q_eval_elementary_handles[term->specific](var, term);
}

int fmi1_xml_q_filter_variable(fmi1_xml_variable_t* var, fmi1_xml_q_expression_t* expr) {
    size_t cur, len = jm_vector_get_size(jm_voidp)(&expr->expression);
	jm_vector(jm_voidp)* stack = &expr->stack;
	fmi1_xml_q_terminal_t * term;
    for(cur = 0; cur < len; cur++) {
        fmi1_xml_q_terminal_t *argL, *argR;
        size_t curlen = jm_vector_get_size(jm_voidp)(stack);

		term = (fmi1_xml_q_terminal_t *)jm_vector_get_item(jm_voidp)(&expr->expression, cur);
        argL = (curlen > 0) ? (fmi1_xml_q_terminal_t *)jm_vector_get_item(jm_voidp)(stack,curlen -1):0;
        argR = (curlen > 1) ? (fmi1_xml_q_terminal_t *)jm_vector_get_item(jm_voidp)(stack,curlen -2):0;

        switch(term -> kind) {
        case fmi1_xml_q_term_enu_AND:
            assert(argL && argR);
            jm_vector_resize(jm_voidp)(stack, curlen -2);
            if((argL->kind == fmi1_xml_q_term_enu_FALSE) || (argR->kind == fmi1_xml_q_term_enu_FALSE))
                jm_vector_push_back(jm_voidp)(stack, &expr->termFalse);
            else {
                jm_vector_push_back(jm_voidp)(stack, (fmi1_xml_evaluate_terminal(var, argL) && fmi1_xml_evaluate_terminal(var, argR))?   &expr->termTrue: &expr->termFalse );
            }
            break;
        case fmi1_xml_q_term_enu_OR:
            assert(argL && argR);
            jm_vector_resize(jm_voidp)(stack, curlen -2);
            if((argL->kind == fmi1_xml_q_term_enu_TRUE) || (argR->kind == fmi1_xml_q_term_enu_TRUE))
                jm_vector_push_back(jm_voidp)(stack, &expr->termTrue);
            else {
                jm_vector_push_back(jm_voidp)(stack, (fmi1_xml_evaluate_terminal(var, argL) || fmi1_xml_evaluate_terminal(var, argR))?   &expr->termTrue: &expr->termFalse);
            }
            break;
        case fmi1_xml_q_term_enu_NOT:
			assert(argL);
            jm_vector_resize(jm_voidp)(stack, curlen -1);
            if(argL->kind == fmi1_xml_q_term_enu_TRUE)
                jm_vector_push_back(jm_voidp)(stack, &expr->termFalse);
			else if(argL->kind == fmi1_xml_q_term_enu_TRUE)
                jm_vector_push_back(jm_voidp)(stack, &expr->termTrue);
            else {
                jm_vector_push_back(jm_voidp)(stack, (fmi1_xml_evaluate_terminal(var, argL)?   &expr->termFalse: &expr->termTrue));
            }
			break;
		case fmi1_xml_q_term_enu_LP:
		case fmi1_xml_q_term_enu_RP:
			assert(0);
			break;
        default:
            jm_vector_push_back(jm_voidp)(stack, term); /* only evaluate when needed. push as is at first */
        }
    }
    assert(jm_vector_get_size(jm_voidp)(stack) == 1);
	
    term = (fmi1_xml_q_terminal_t *)jm_vector_get_item(jm_voidp)(stack,0);
    if(term->kind == fmi1_xml_q_term_enu_FALSE) return 0;
	assert(term->kind == fmi1_xml_q_term_enu_TRUE);
    return 1;
}

fmi1_xml_q_expression_t* fmi1_xml_alloc_expression(jm_string query) {
	return 0;
}

int fmi1_xml_q_parse_elementary(fmi1_xml_q_context_t* context, fmi1_xml_q_terminal_t* term) {
	return 0;
}

void fmi1_xml_q_init_context(fmi1_xml_q_context_t* c, jm_callbacks* cb) {
	size_t l = jm_vector_init(jm_name_ID_map_t)(&c->elementary_map, fmi1_xml_elementary_enu_num, cb);
	assert(l);
	for(l = 0; l < fmi1_xml_elementary_enu_num; l++) {
		jm_vector_set_item(jm_name_ID_map_t)(&c->elementary_map, l,fmi1_xml_q_elementary_map[l]);
	}
	jm_vector_qsort(jm_name_ID_map_t)(&c->elementary_map,jm_compare_name);
	c->query = 0;
	c->qlen = 0;
	c->curCh = 0;
	jm_vector_init(char)(&c->buf,0,cb);

	{
		fmi1_xml_q_expression_t* expr = &c->expr;
		jm_vector_init(jm_voidp)(&expr->expression,0,cb);
		jm_vector_init(jm_voidp)(&expr->stack,0,cb);
		expr->termFalse.kind = 	fmi1_xml_q_term_enu_FALSE;
		expr->termTrue.kind = 	fmi1_xml_q_term_enu_TRUE;
		jm_vector_init(fmi1_xml_q_terminal_t)(&expr->terms,0,cb);
		jm_vector_init(char)(&expr->strbuf, 0, cb);
	}
}

void fmi1_xml_q_free_context_data(fmi1_xml_q_context_t* c){
	fmi1_xml_q_expression_t* expr = &c->expr;
	jm_vector_free_data(jm_name_ID_map_t)(&c->elementary_map);
	jm_vector_free_data(char)(&c->buf);
	jm_vector_free_data(jm_voidp)(&expr->expression);
	jm_vector_free_data(jm_voidp)(&expr->stack);
	jm_vector_free_data(fmi1_xml_q_terminal_t)(&expr->terms);
	jm_vector_free_data(char)(&expr->strbuf);
}

int fmi1_xml_q_parse_terminal(fmi1_xml_q_context_t* context, fmi1_xml_q_terminal_t** ppterm) {
	int offset = 0;
    fmi1_xml_q_terminal_t* pterm;
	jm_string startCh = context->query + context->curCh;
	jm_string cur = startCh;
	pterm = jm_vector_resize1(fmi1_xml_q_terminal_t)(&context->expr.terms);
    if(!pterm) return -1;
	*ppterm = pterm;
    fmi1_xml_q_skip_space(&cur);
    switch(*cur) {
    case '(':
		pterm->kind = fmi1_xml_q_term_enu_LP;
		break;
    case ')':
		pterm->kind = fmi1_xml_q_term_enu_RP;
		break;
    case '&':
		pterm->kind = fmi1_xml_q_term_enu_AND;
		break;
    case '|':
		pterm->kind = fmi1_xml_q_term_enu_OR;
		break;
    case '!':
		pterm->kind = fmi1_xml_q_term_enu_NOT;
		break;
    case 0:
		pterm->kind = fmi1_xml_q_term_enu_END;
		break;
    default:
        fmi1_xml_q_parse_elementary(context, pterm);
    }
	fmi1_xml_q_skip_space(&cur);
	return (int)(cur - startCh);
}

int fmi1_xml_q_parse_query(fmi1_xml_q_context_t* context, jm_string query) {
	fmi1_xml_q_expression_t* expr = &context->expr;
    int qlen = strlen(query);
	int offset = 0, curCh = 0;
	int expectOperand = 1;
	size_t stacklen = 0;
    fmi1_xml_q_terminal_t* stackTop = 0;

	context->query = query;
	context->qlen = qlen;
	if(jm_vector_reserve(char)(&context->buf, qlen) < (size_t)qlen) return -1;
	if(jm_vector_reserve(char)(&context->expr.strbuf, qlen) < (size_t)qlen) return -1;

    while(curCh < qlen) {
        fmi1_xml_q_terminal_t* term;
        size_t explen = jm_vector_get_size(jm_voidp)(&expr->expression);
        fmi1_xml_q_terminal_t* expTop =  explen? (fmi1_xml_q_terminal_t*)jm_vector_get_item(jm_voidp)(&expr->expression,explen -1):0;

		offset = fmi1_xml_q_parse_terminal(context,  &term);

        if(offset < 0) return -curCh;		

		stacklen = jm_vector_get_size(jm_voidp)(&expr->stack);
        stackTop =  stacklen ? (fmi1_xml_q_terminal_t*)jm_vector_get_item(jm_voidp)(&expr->stack,stacklen -1):(fmi1_xml_q_terminal_t*)0;

        switch(term -> kind) {
        case fmi1_xml_q_term_enu_LP:
			if(!expectOperand) return -curCh;
            jm_vector_push_back(jm_voidp)(&expr->stack, term);
            break;
        case fmi1_xml_q_term_enu_RP:
			if(expectOperand) return -curCh;
            while(stackTop && (stackTop->kind != fmi1_xml_q_term_enu_LP)) {
                jm_vector_push_back(jm_voidp)(&expr->expression, stackTop);
                stacklen--;
                jm_vector_resize(jm_voidp)(&expr->stack, stacklen);
                stackTop =  stacklen? (fmi1_xml_q_terminal_t*)jm_vector_get_item(jm_voidp)(&expr->stack,stacklen -1):0;
            }
            if(!stackTop) return -curCh;
            jm_vector_resize(jm_voidp)(&expr->stack, stacklen -1);
            break;

        case fmi1_xml_q_term_enu_AND:
			if(expectOperand) return -curCh;
			expectOperand = 1;
            if(!expTop) return -curCh;
            if(stackTop && (stackTop->kind == fmi1_xml_q_term_enu_AND))
                    jm_vector_push_back(jm_voidp)(&expr->expression, term);
                else
                    jm_vector_push_back(jm_voidp)(&expr->stack, term);
            break;
        case fmi1_xml_q_term_enu_OR:
			if(expectOperand) return -curCh;
			expectOperand = 1;
            if(!expTop) return -curCh;
            while(stackTop && ((stackTop->kind == fmi1_xml_q_term_enu_AND)||(stackTop->kind == fmi1_xml_q_term_enu_OR))) {
                jm_vector_push_back(jm_voidp)(&expr->expression, stackTop);
                jm_vector_resize(jm_voidp)(&expr->stack, stacklen -1);
                stacklen--;
                stackTop =  stacklen? jm_vector_get_item(jm_voidp)(&expr->stack,stacklen -1):0;
            }
            jm_vector_push_back(jm_voidp)(&expr->stack, term);
            break;
        default:
			if(!expectOperand) return -curCh;
			expectOperand = 0;
            jm_vector_push_back(jm_voidp)(&expr->expression, term);

        }
		curCh += offset;
		context->curCh = curCh;
    }
	if(expectOperand) return -curCh;
    while(stackTop && (stackTop->kind != fmi1_xml_q_term_enu_LP)) {
		jm_vector_push_back(jm_voidp)(&expr->expression, stackTop);
        jm_vector_resize(jm_voidp)(&expr->stack, stacklen -1);
        stacklen--;
        stackTop =  stacklen? (fmi1_xml_q_terminal_t*)jm_vector_get_item(jm_voidp)(&expr->stack,stacklen -1):0;
    }
    if(!stackTop) return -curCh;
    jm_vector_resize(jm_voidp)(&expr->stack, stacklen -1);

    return 0;
}

#define JM_TEMPLATE_INSTANCE_TYPE fmi1_xml_q_terminal_t
#include "jm_vector_template.h"
