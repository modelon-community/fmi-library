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

#ifndef FMI_XML_COSIM_H
#define FMI_XML_COSIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fmi1_xml_model_description.h"

const char* fmi1_xml_get_entry_point(fmi1_xml_model_description_t* );
const char* fmi1_xml_get_mime_type(fmi1_xml_model_description_t* );
int fmi1_xml_get_manual_start(fmi1_xml_model_description_t* );

size_t fmi1_xml_get_number_of_additional_models(fmi1_xml_model_description_t* md);

const char* fmi1_xml_get_additional_model_name(fmi1_xml_model_description_t* md, size_t index);

#ifdef __cplusplus
}
#endif
#endif /* FMI_XML_COSIM_H */
