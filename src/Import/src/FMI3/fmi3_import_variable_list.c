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

#include "fmi3_import_impl.h"
#include "fmi3_import_variable_list_impl.h"

fmi3_import_variable_list_t* fmi3_import_alloc_variable_list(fmi3_import_t* fmu, size_t size) {
    jm_callbacks* cb = fmu->callbacks;
    fmi3_import_variable_list_t* vl = (fmi3_import_variable_list_t*)cb->malloc(sizeof(fmi3_import_variable_list_t));
    if(!vl) return 0;
    vl->vr = 0;
    vl->fmu = fmu;
    if(jm_vector_init(jm_voidp)(&vl->variables,size,cb) < size) {
        fmi3_import_free_variable_list(vl);
        return 0;
    }
    return vl;
}

void fmi3_import_free_variable_list(fmi3_import_variable_list_t* vl) {
    jm_callbacks* cb;
    if(!vl) return;
    cb = vl->variables.callbacks;
    cb->free(vl->vr);
    jm_vector_free_data(jm_voidp)(&vl->variables);
    cb->free(vl);
}

/* Get number of variables in a list */
size_t  fmi3_import_get_variable_list_size(fmi3_import_variable_list_t* vl) {
    if(vl)
        return jm_vector_get_size(jm_voidp)(&vl->variables);
    else
        return 0;
}

/* Make a copy */
fmi3_import_variable_list_t* fmi3_import_clone_variable_list(fmi3_import_variable_list_t* vl) {
    fmi3_import_variable_list_t* copy = fmi3_import_alloc_variable_list(vl->fmu, fmi3_import_get_variable_list_size(vl));
    if(!copy) return 0;
    jm_vector_copy(jm_voidp)(&copy->variables, &vl->variables);
    return copy;
}

fmi3_import_variable_list_t* fmi3_import_join_var_list(fmi3_import_variable_list_t* a, fmi3_import_variable_list_t* b) {
    size_t asize = fmi3_import_get_variable_list_size(a);
    size_t bsize = fmi3_import_get_variable_list_size(b);
    size_t joinSize = asize + bsize;
    fmi3_import_variable_list_t* list = fmi3_import_alloc_variable_list(a->fmu,joinSize);
    if(!list) {
        return list;
    }
    jm_vector_copy(jm_voidp)(&list->variables,&a->variables);
    jm_vector_resize(jm_voidp)(&list->variables,joinSize);
    memcpy((void*)jm_vector_get_itemp(jm_voidp)(&list->variables,asize), (void*)jm_vector_get_itemp(jm_voidp)(&b->variables,0), sizeof(jm_voidp)*bsize);
    return list;
}

fmi3_import_variable_list_t* fmi3_import_create_var_list(fmi3_import_t* fmu,fmi3_import_variable_t* v) {
    fmi3_import_variable_list_t* list = fmi3_import_alloc_variable_list(fmu,1);
    if(!list ) {
        return list;
    }
    jm_vector_set_item(jm_voidp)(&list->variables,0,v);
    return list;
}

fmi3_import_variable_list_t* fmi3_import_append_to_var_list(fmi3_import_variable_list_t* list, fmi3_import_variable_t* v) {
    fmi3_import_variable_list_t* out = fmi3_import_alloc_variable_list(list->fmu, fmi3_import_get_variable_list_size(list)+1);
    if(!out) return 0;
    jm_vector_copy(jm_voidp)(&out->variables,&list->variables);
    jm_vector_push_back(jm_voidp)(&out->variables, v);
    return out;
}

fmi3_import_variable_list_t* fmi3_import_prepend_to_var_list(fmi3_import_variable_list_t* list, fmi3_import_variable_t* v) {
    size_t lsize = fmi3_import_get_variable_list_size(list);
    fmi3_import_variable_list_t* out = fmi3_import_alloc_variable_list(list->fmu, lsize+1);
    if(!out) return 0;
    jm_vector_set_item(jm_voidp)(&out->variables,0,v);
    memcpy((void*)jm_vector_get_itemp(jm_voidp)(&out->variables,1), (void*)jm_vector_get_itemp(jm_voidp)(&list->variables,0), sizeof(jm_voidp)*lsize);
    return out;
}

jm_status_enu_t fmi3_import_var_list_push_back(fmi3_import_variable_list_t* list, fmi3_import_variable_t* v) {
    if(!jm_vector_push_back(jm_voidp)(&list->variables, v)) return jm_status_error;
    return jm_status_success;
}

/* Get a pointer to the list of the value references for all the variables */
const fmi3_value_reference_t* fmi3_import_get_value_reference_list(fmi3_import_variable_list_t* vl) {
    if(!vl->vr) {
        jm_callbacks* cb = vl->fmu->callbacks;
        size_t i, nv = fmi3_import_get_variable_list_size(vl);
        vl->vr = (fmi3_value_reference_t*)cb->malloc(nv * sizeof(fmi3_value_reference_t));
        if(vl->vr) {
            for(i = 0; i < nv; i++) {
                vl->vr[i] = fmi3_xml_get_variable_vr(fmi3_import_get_variable(vl, i));
            }
        }
        else return 0;
    }
    return vl->vr;
}

/* Get a single variable from the list*/
fmi3_import_variable_t* fmi3_import_get_variable(fmi3_import_variable_list_t* vl, size_t  index) {
    if(index >= fmi3_import_get_variable_list_size(vl))
        return 0;
    else
        return (fmi3_import_variable_t*)jm_vector_get_item(jm_voidp)(&vl->variables, index);
}

/* Operations on variable lists. Every operation creates a new list. */
/* Select sub-lists */
fmi3_import_variable_list_t* fmi3_import_get_sublist(fmi3_import_variable_list_t* vl, size_t  fromIndex, size_t  toIndex) {
    fmi3_import_variable_list_t* out;
    size_t size, i;
    if(fromIndex > toIndex) return 0;
    if(toIndex >=  fmi3_import_get_variable_list_size(vl)) return 0;
    size = toIndex - fromIndex + 1;
    out = fmi3_import_alloc_variable_list(vl->fmu, size);
    if(!out ) return 0;
    for(i=0; i < size; i++) {
        jm_vector_set_item(jm_voidp)(&out->variables, i, jm_vector_get_item(jm_voidp)(&vl->variables, fromIndex+i));
    }
    return out;
}

/* fmi3_import_filter_variables calls  the provided 'filter' function on every variable in the list.
  It returns a sub-list list with the variables for which filter returned non-zero value. */
fmi3_import_variable_list_t* fmi3_import_filter_variables(fmi3_import_variable_list_t* vl, fmi3_import_variable_filter_function_ft filter, void* context) {
    size_t nv, i;
    fmi3_import_variable_list_t* out = fmi3_import_alloc_variable_list(vl->fmu, 0);
    if(!out) return 0; /* out of memory */
    nv = fmi3_import_get_variable_list_size(vl);
    for(i=0; i < nv;i++) {
        fmi3_import_variable_t* variable = fmi3_import_get_variable(vl, i);
        if(filter(variable, context))
            if(!jm_vector_push_back(jm_voidp)(&out->variables, variable))
                break;
    }
    if(i != nv) {
        fmi3_import_free_variable_list(out);
        out = 0;
    }
    return out;
}
