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

#ifndef FMI_VERSION_H
#define FMI_VERSION_H
#include <fmilib_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
	@file fmi_version.h Enum defining supported FMI versions.

	*/
/** \addtogroup jm_utils
  * @{
*/

/** \brief Suported versions of FMI standard */
typedef enum
{ 
	fmi_version_unknown_enu = 0,
	fmi_version_1_enu,
	fmi_version_2_0_enu,
	fmi_version_unsupported_enu
} fmi_version_enu_t;

/** Convert version enum into string */
FMILIB_EXPORT
const char * fmi_version_to_string(fmi_version_enu_t v);

/** @} */
#ifdef __cplusplus
}
#endif

/* JM_TYPES_H */
#endif
