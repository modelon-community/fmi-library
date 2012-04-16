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

#ifndef FMI1_IMPORT_VARIABLELISTIMPL_H
#define FMI1_IMPORT_VARIABLELISTIMPL_H

#include <jm_vector.h>
#include <FMI1/fmi1_import.h>
#include <FMI1/fmi1_import_variable_list.h>

#ifdef __cplusplus
extern "C" {
#endif

struct fmi1_import_variable_list_t {
	fmi1_import_t* fmu;
    jm_vector(jm_voidp) variables;
    jm_vector(size_t)* vr;
};

/* Allocate an empty list */
fmi1_import_variable_list_t* fmi1_import_alloc_variable_list(fmi1_import_t* fmu, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* FMI1_XML_VARIABLELISTIMPL_H */
