/*
// This file is supposed to be included into a C-file that instantiate the template.
// jm_vector.h must be included before this file.
// It expects JM_TEMPLATE_INSTANCE_TYPE to be defined to the template type to be instantiated.
*/

#include <stdlib.h>
#include <string.h>
#include "jm_vector.h"

#ifndef JM_TEMPLATE_INSTANCE_TYPE
#error "JM_TEMPLATE_INSTANCE_TYPE must be defined before including this file"
#endif

jm_vector(JM_TEMPLATE_INSTANCE_TYPE) * jm_vector_alloc(JM_TEMPLATE_INSTANCE_TYPE) (size_t initSize, jm_context* c) {
        int reserve;
        jm_context* cc;
        jm_vector(JM_TEMPLATE_INSTANCE_TYPE) * v;
        if(c)
            cc = c;
        else
            cc = jm_get_default_context();

        if(initSize > jm_vector_preallocate_items_num) {
            v = (jm_vector(JM_TEMPLATE_INSTANCE_TYPE)*)cc->allocate(
                        sizeof(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)) +
                        sizeof(JM_TEMPLATE_INSTANCE_TYPE) * (initSize -jm_vector_preallocate_items_num),
                        1);
            if(!v) return 0;
            v->allocated = initSize;
        }
        else {
            v = (jm_vector(JM_TEMPLATE_INSTANCE_TYPE)*)cc->allocate(1,sizeof(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)));
            if(!v) return 0;
            v->allocated = jm_vector_preallocate_items_num;
        }
        v->items = v->preallocated;
        v->size = initSize;
        return v;
}

void jm_vector_free(JM_TEMPLATE_INSTANCE_TYPE) (jm_vector(JM_TEMPLATE_INSTANCE_TYPE) * a) {
    jm_vector_free_data(JM_TEMPLATE_INSTANCE_TYPE)(a);
    a->context->free(a);
}

size_t jm_vector_init(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* a, size_t initSize, jm_context* c) {
        int reserve;
        if(c)
            a->context = c;
        else
            a->context = jm_get_default_context();
        a->items = a->preallocated;
        a->size = 0;
        a->allocated = jm_vector_preallocate_items_num;

        if(initSize > a->size)
            return jm_vector_resize(JM_TEMPLATE_INSTANCE_TYPE)(a, initSize);
        return 0;
}

size_t jm_vector_resize(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* a, size_t size) {
        if(size > a->allocated)  {
            if(jm_vector_reserve(JM_TEMPLATE_INSTANCE_TYPE)(a, size) < size) {
                a->size = a->allocated;
                return a->allocated;
            }
        }
        a->size = size;
        return size;
}

size_t jm_vector_reserve(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* a, size_t size) {
        void* newmem;
        if(size <= a->allocated) return a->allocated;
        newmem = a->context->allocate(size,sizeof(JM_TEMPLATE_INSTANCE_TYPE));
        if(!newmem) return a->allocated;
        memcpy(newmem, a->items, a->size * sizeof(JM_TEMPLATE_INSTANCE_TYPE));
        if(a->items !=  a->preallocated) a->context->free(a->items);
        a->items = newmem;
        a->allocated = size;
        return a->allocated;
}

size_t jm_vector_copy(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* destination, jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* source) {
        if(jm_vector_resize(JM_TEMPLATE_INSTANCE_TYPE)(destination, source->size) < source->size)
                return 0;
        memcpy(destination->items, source->items, sizeof(JM_TEMPLATE_INSTANCE_TYPE)*source->size);
        return destination->size;
}

JM_TEMPLATE_INSTANCE_TYPE* jm_vector_insert(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* a, size_t index, JM_TEMPLATE_INSTANCE_TYPE item) {
        int reserve;
        JM_TEMPLATE_INSTANCE_TYPE* pitem;
        if(index >= a->size) return 0;
        if(a->size == a->allocated) {
                if(a->allocated > jm_vector_max_memory_chunk)
                        reserve = jm_vector_max_memory_chunk + a->allocated;
                else
                        reserve = a->allocated * 2;
                if( jm_vector_reserve(JM_TEMPLATE_INSTANCE_TYPE)(a, reserve) != reserve) return 0;
        }
        assert(a->size < a->allocated);
        memmove(a->items+index+1,a->items+index, a->size - index);
        a->items[index] = item;
        pitem = &(a->items[index]);
        a->size++;
        return pitem;
}

JM_TEMPLATE_INSTANCE_TYPE* jm_vector_push_back(JM_TEMPLATE_INSTANCE_TYPE) (jm_vector(JM_TEMPLATE_INSTANCE_TYPE) * a, JM_TEMPLATE_INSTANCE_TYPE item) {
        int reserve;
        JM_TEMPLATE_INSTANCE_TYPE* pitem;
        if(a->size == a->allocated) {
                if(a->allocated > jm_vector_max_memory_chunk)
                        reserve = jm_vector_max_memory_chunk + a->allocated;
                else
                        reserve = a->allocated * 2;
                if( jm_vector_reserve(JM_TEMPLATE_INSTANCE_TYPE)(a, reserve) != reserve) return 0;
        }
        assert(a->size < a->allocated);
        a->items[a->size] = item;
        pitem = &(a->items[a->size]);
        a->size++;
        return pitem;
}

void jm_vector_foreach(JM_TEMPLATE_INSTANCE_TYPE)(jm_vector(JM_TEMPLATE_INSTANCE_TYPE)* a, void (*f)(JM_TEMPLATE_INSTANCE_TYPE, void*), void * data) {
        int i;
        for(i = 0; i < jm_vector_get_size(JM_TEMPLATE_INSTANCE_TYPE)(a); i++) f(jm_vector_get_item(JM_TEMPLATE_INSTANCE_TYPE)(a, i), data);
}
