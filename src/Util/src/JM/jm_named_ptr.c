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
#include "JM/jm_callbacks.h"
#include "JM/jm_named_ptr.h"

/**
 * See documentation for 'jm_named_alloc_v'.
 */
jm_named_ptr jm_named_alloc(const char* name, size_t size, size_t nameoffset, jm_callbacks* c) {
    jm_named_ptr out;
    size_t namelen = strlen(name);
    size_t sizefull = size + namelen;
    out.ptr = c->malloc(sizefull);
    out.name = 0;
    if(out.ptr) {
        char* outname;
        outname = out.ptr;
        outname += nameoffset;
        if(namelen)
            memcpy(outname, name, namelen);
        outname[namelen] = 0;
        out.name = outname;
    }
    return out;
}

/**
 * Returns a jm_named_ptr where the memory for the .ptr field has been allocated
 * and the name set.
 * 
 * The caller needs to verify that the .ptr field is not NULL.
 *
 * The allocated memory must be able to store 'name', and 'nameoffset' must give
 * the offset to that address.
 * 
 * @param name:
 *      The name.
 * @param size:
 *      Size of memory for the .ptr (just like for malloc). Do NOT take into account size
 *      for the name since this function handles that.
 * @param nameoffset:
 *      Write the name at offset from the start of the requested 'size'.
 *      Impl. note: this needs to be calculated by caller, because it's possible
 *      to get different byte padding of the struct, and it allows the function
 *      to be used without structs that end with a name field.
 */
jm_named_ptr jm_named_alloc_v(jm_vector(char)* name, size_t size, size_t nameoffset, jm_callbacks* c) {
    jm_named_ptr out;
    size_t namelen = jm_vector_get_size(char)(name);
    size_t sizefull = size + namelen;
    out.ptr = c->malloc(sizefull); /* Optimization: malloc once instead of twice */
    out.name = NULL;
    if (out.ptr) {
        char* outname = out.ptr;
        outname += nameoffset;
        if (namelen) {
            memcpy(outname, jm_vector_get_itemp(char)(name, 0), namelen);
        }
        outname[namelen] = '\0';
        out.name = outname;
    }
    return out;
}

#define JM_TEMPLATE_INSTANCE_TYPE jm_named_ptr
#include "JM/jm_vector_template.h"
