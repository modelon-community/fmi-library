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

#ifndef JM_STRING_SET_H
#define JM_STRING_SET_H

#include <string.h>

#include "jm_types.h"
#include "jm_vector.h"
#ifdef __cplusplus
extern "C" {
#endif
/** \file jm_string_set.h Definition of ::jm_string_set and supporting functions
	*
	* \addtogroup jm_utils
	* @{
	*    \addtogroup jm_string_set_group
	* @}
	*/

	/** \addtogroup jm_string_set_group A set of strings
	 @{
	*/

/** 
	\brief Set of string is based on a vector	

*/
typedef struct jm_vector_jm_string jm_string_set; /* equivalent to "typedef jm_vector(jm_string) jm_string_set" which Doxygen does not understand */

/**
\brief Find a string in a set.

\param s A string set.
\param str Search string.
\return If found returns a pointer to the string saved in the set. If not found returns NULL.
*/
static jm_string jm_string_set_find(jm_string_set* s, jm_string str) {
    jm_string* found = jm_vector_find(jm_string)(s,&str,jm_compare_string);
    if(found) return *found;
    return 0;
}

/**
*  \brief Put an element in the set if it is not there yet.
*
*  @param s A string set.
*  \param str String to put.
*  @return A pointer to the inserted (or found) element or zero pointer if failed.
*/
static jm_string jm_string_set_put(jm_string_set* s, jm_string str) {
    jm_string found = jm_string_set_find(s, str);
    if(found) return found;
    {
        char* newstr = 0;
        size_t len = strlen(str) + 1;
        jm_string* pnewstr = jm_vector_push_back(jm_string)(s, newstr);
        if(pnewstr) *pnewstr = newstr = s->callbacks->malloc(len);
        if(!pnewstr || !newstr) return 0;
        memcpy(newstr, str, len);
        jm_vector_qsort(jm_string)(s, jm_compare_string);
        found = newstr;
    }
    return found;
}
/** @}
	*/

#ifdef __cplusplus
}
#endif

#endif /* JM_STRING_SET_H */
