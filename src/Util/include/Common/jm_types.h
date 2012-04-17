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

#ifndef JM_TYPES_H
#define JM_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif

/**
	@file jm_types.h Types used in the Utils module.

	*/
/** \defgroup jm_utils Utilities 
  * @{
*/
/** \brief A constant string.*/
typedef const char* jm_string;
/** \brief A void pointer.*/
typedef void* jm_voidp;

/** \brief Mapping between a string and an integer ID */
typedef struct jm_name_ID_map_t {
    jm_string name;
    unsigned int ID;
} jm_name_ID_map_t;

/** \brief Return status codes */
typedef enum {	
	jm_status_error = -1,
	jm_status_success = 0,
	jm_status_warning = 1
} jm_status_enu_t;

/** \brief Log levels supported via the logger functions in ::jm_callbacks */
typedef enum {	
	jm_log_level_all, /* "all" must be first in this enum*/
	jm_log_level_info,
	jm_log_level_warning,
	jm_log_level_error,
	jm_log_level_fatal,
	jm_log_level_nothing /* "nothing" must be last in this enum*/
} jm_log_level_enu_t;

/** @} */
#ifdef __cplusplus
}
#endif

/* JM_TYPES_H */
#endif
