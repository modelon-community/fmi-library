/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common/jm_wc_match.h"

/* The code is inspired by: http://cm.bell-labs.com/cm/cs/tpop/code.html
	You may use this code for any purpose, as long as you leave the 
	copyright notice and book citation attached (see below). 
*/
/* Copyright (C) 1999 Lucent Technologies */
/* Excerpted from 'The Practice of Programming' */
/* by Brian W. Kernighan and Rob Pike */

/* jm_wc_matchstar: try to match any tail */
int jm_wc_matchstar(const char *pattern, const char *str)
{
	const char * tail = str;
	if(*tail == '\0') return 1;
	do {	
		if (jm_wc_match(pattern, tail))
			return 1;
		tail++;
	} while (*tail != '\0');
	return 0;
}

int	jm_wc_match(const char* pattern, const char* str) {
	switch(pattern[0]) {
	case '*':
			return jm_wc_matchstar(pattern+1, str);
	case '?':
			if(*str)
				return jm_wc_match(pattern+1, str+1);
			else 
				return 0;
	case '\0':
			return (*str == '\0');
	default:
			return ((*str == *pattern) && (jm_wc_match(pattern+1, str+1)));
	}
}
