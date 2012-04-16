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

#include <string.h>

#include "fmi1_import_impl.h"

const char* fmi1_import_get_vendor_name(fmi1_import_vendor_t* v) {
	return fmi1_xml_get_vendor_name(v);
}

unsigned int  fmi1_import_get_number_of_vendor_annotations(fmi1_import_vendor_t* v) {
    return fmi1_xml_get_number_of_vendor_annotations(v);
}

fmi1_import_annotation_t* fmi1_import_get_vendor_annotation(fmi1_import_vendor_t* v, unsigned int  index) {
	return fmi1_xml_get_vendor_annotation(v, index);
}

const char* fmi1_import_get_annotation_name(fmi1_import_annotation_t* a) {
	return fmi1_xml_get_annotation_name(a);
}

const char* fmi1_import_get_annotation_value(fmi1_import_annotation_t* a) {
	return fmi1_xml_get_annotation_value(a);
}
