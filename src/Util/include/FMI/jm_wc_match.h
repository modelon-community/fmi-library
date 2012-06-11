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

#ifndef JM_WC_MATCH_H
#define JM_WC_MATCH_H
#ifdef __cplusplus
extern "C" {
#endif


/**
* \file jm_wc_match.h
* \brief Wildcard matching function.
* \addtogroup jm_utils
	* @{
*/

/** Check if the string str matches pattern string (with '*' and '?').
   @return 0 if not matched; 1 - if matched.
*/
int	jm_wc_match(const char* pattern, const char* str);

/**
* @}
*/

#ifdef __cplusplus
}
#endif

/* JM_WC_MATCH_H */
#endif
