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
#include <string.h>

#include "fmi1_import_impl.h"
#include "fmi1_import_variable_list_impl.h"

fmi1_import_variable_list_t* fmi1_import_alloc_variable_list(fmi1_import_t* fmu, size_t size) {
	jm_callbacks* cb = fmu->callbacks;
	fmi1_import_variable_list_t* vl = (fmi1_import_variable_list_t*)cb->malloc(sizeof(fmi1_import_variable_list_t));
    if(!vl) return 0;
    vl->vr = 0;
	vl->fmu = fmu;
    if(jm_vector_init(jm_voidp)(&vl->variables,size,cb) < size) {
        fmi1_import_free_variable_list(vl);
        return 0;
    }
    return vl;
}

void fmi1_import_free_variable_list(fmi1_import_variable_list_t* vl) {
    jm_callbacks* cb;
	if(!vl) return;
	cb = vl->variables.callbacks;
	cb->free(vl->vr);
    jm_vector_free_data(jm_voidp)(&vl->variables);
    cb->free(vl);
}

/* Get number of variables in a list */
size_t  fmi1_import_get_variable_list_size(fmi1_import_variable_list_t* vl) {
	if(vl)
		return jm_vector_get_size(jm_voidp)(&vl->variables);
	else
		return 0;
}

/* Make a copy */
fmi1_import_variable_list_t* fmi1_import_clone_variable_list(fmi1_import_variable_list_t* vl) {
    fmi1_import_variable_list_t* copy;
	if(!vl) return 0;
	copy = fmi1_import_alloc_variable_list(vl->fmu, fmi1_import_get_variable_list_size(vl));
    if(!copy) return 0;
    jm_vector_copy(jm_voidp)(&copy->variables, &vl->variables);
    return copy;
}

fmi1_import_variable_list_t* fmi1_import_join_var_list(fmi1_import_variable_list_t* a, fmi1_import_variable_list_t* b) {
    size_t asize = fmi1_import_get_variable_list_size(a);
    size_t bsize = fmi1_import_get_variable_list_size(b);
    size_t joinSize = asize + bsize;
    fmi1_import_variable_list_t* list = fmi1_import_alloc_variable_list(a->fmu,joinSize);
    if(!list) {
        return list;
    }
	if(a)
		jm_vector_copy(jm_voidp)(&list->variables,&a->variables);
	if(b) {
		jm_vector_resize(jm_voidp)(&list->variables,joinSize);
		memcpy((void*)jm_vector_get_itemp(jm_voidp)(&list->variables,asize), (void*)jm_vector_get_itemp(jm_voidp)(&b->variables,0), sizeof(jm_voidp)*bsize);
	}
    return list;
}

fmi1_import_variable_list_t* fmi1_import_create_var_list(fmi1_import_t* fmu,fmi1_import_variable_t* v) {
    fmi1_import_variable_list_t* list = fmi1_import_alloc_variable_list(fmu,1);
    if(!list ) {
        return list;
    }
    jm_vector_set_item(jm_voidp)(&list->variables,0,v);
    return list;
}

fmi1_import_variable_list_t* fmi1_import_append_to_var_list(fmi1_import_variable_list_t* list, fmi1_import_variable_t* v) {
    fmi1_import_variable_list_t* out = fmi1_import_alloc_variable_list(list->fmu, fmi1_import_get_variable_list_size(list)+1);
    if(!out) return 0;
    jm_vector_copy(jm_voidp)(&out->variables,&list->variables);
    jm_vector_push_back(jm_voidp)(&out->variables, v);
    return out;
}

fmi1_import_variable_list_t* fmi1_import_prepend_to_var_list(fmi1_import_variable_list_t* list, fmi1_import_variable_t* v) {
    size_t lsize = fmi1_import_get_variable_list_size(list);
    fmi1_import_variable_list_t* out = fmi1_import_alloc_variable_list(list->fmu, lsize+1);
    if(!out) return 0;
    jm_vector_set_item(jm_voidp)(&out->variables,0,v);
    memcpy((void*)jm_vector_get_itemp(jm_voidp)(&out->variables,1), (void*)jm_vector_get_itemp(jm_voidp)(&list->variables,0), sizeof(jm_voidp)*lsize);
    return out;
}

jm_status_enu_t fmi1_import_var_list_push_back(fmi1_import_variable_list_t* list, fmi1_import_variable_t* v) {
    if(!jm_vector_push_back(jm_voidp)(&list->variables, v)) return jm_status_error;
    return jm_status_success;
}

/* Get a pointer to the list of the value references for all the variables */
const fmi1_value_reference_t* fmi1_import_get_value_referece_list(fmi1_import_variable_list_t* vl) {
    if(!vl->vr) {
		jm_callbacks* cb = vl->fmu->callbacks;
        size_t nv = fmi1_import_get_variable_list_size(vl);
		unsigned i = (unsigned)nv;
		assert( i == nv);
		vl->vr = (fmi1_value_reference_t*)cb->malloc(nv * sizeof(fmi1_value_reference_t));
        if(vl->vr) {
            for(i = 0; i < nv; i++) {
				vl->vr[i] = fmi1_xml_get_variable_vr(fmi1_import_get_variable(vl, i));
            }
        }
		else return 0;
    }
    return vl->vr;
}

/* Get a single variable from the list*/
fmi1_import_variable_t* fmi1_import_get_variable(fmi1_import_variable_list_t* vl, unsigned int  index) {
	if(index >= fmi1_import_get_variable_list_size(vl))
		return 0;
	else
		return (fmi1_import_variable_t*)jm_vector_get_item(jm_voidp)(&vl->variables, index);
}

/* Operations on variable lists. Every operation creates a new list. */
/* Select sub-lists */
fmi1_import_variable_list_t* fmi1_import_get_sublist(fmi1_import_variable_list_t* vl, unsigned int  fromIndex, unsigned int  toIndex) {
    fmi1_import_variable_list_t* out;
    size_t size, i;
    if(fromIndex > toIndex) return 0;
    if(toIndex >=  fmi1_import_get_variable_list_size(vl)) return 0;
    size = toIndex - fromIndex + 1;
    out = fmi1_import_alloc_variable_list(vl->fmu, size);
    if(!out ) return 0;
    for(i=0; i < size; i++) {
        jm_vector_set_item(jm_voidp)(&out->variables, i, jm_vector_get_item(jm_voidp)(&vl->variables, fromIndex+i));
    }
    return out;
}

/* fmi1_import_filter_variables calls  the provided 'filter' function on every variable in the list.
  It returns a sub-list list with the variables for which filter returned non-zero value. */
fmi1_import_variable_list_t* fmi1_import_filter_variables(fmi1_import_variable_list_t* vl, fmi1_import_variable_filter_function_ft filter, void* context) {
    size_t nv;
	unsigned i;
    fmi1_import_variable_list_t* out;
    nv = fmi1_import_get_variable_list_size(vl);
	i = (unsigned) nv;
	if(i != nv) return 0; /* this is not really possible */
	out = fmi1_import_alloc_variable_list(vl->fmu, 0);
	if(!out) return 0; /* out of memory */
    for(i=0; i < nv;i++) {
        fmi1_import_variable_t* variable = fmi1_import_get_variable(vl, i);
        if(filter(variable, context))
            if(!jm_vector_push_back(jm_voidp)(&out->variables, variable))
                break;
    }
    if(i != nv) {
        fmi1_import_free_variable_list(out);
        out = 0;
    }
    return out;
}
