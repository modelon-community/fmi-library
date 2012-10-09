#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "config_test.h"
#include <JM/jm_vector.h>
#include <JM/jm_stack.h>

void print_int(int i,void* data) {
    printf("%d\n", i);
}

void print_dbl(double d,void* data) {
    printf("%g\n", d);
}

int compar_int(int* a, int* b) {
    return (*a - *b);
}

int return_code = CTEST_RETURN_SUCCESS;

void log_error(const char* fmt, ...) {
	va_list args;
    va_start (args, fmt);
    vprintf(fmt, args);
    va_end (args);
	return_code = CTEST_RETURN_FAIL;
}

#define TESTVAL 49

int main() {
    int i, k;
    jm_vector(int) stackv;
    jm_stack(double)* s;

    jm_vector(int)* v = &stackv;
    srand(0);
    s = jm_stack_alloc(double)(100, 0 );
#define VINIT_SIZE 5
    jm_vector_init(int)(v,VINIT_SIZE,0);
    jm_vector_zero(int)(v);
    jm_vector_set_item(int)(v, 2, TESTVAL);
    for( i = 0; i < 32; i++) {
        int x = i+TESTVAL;
		int top;
        jm_vector_push_back(int)(v,x);
        jm_stack_push(double)(s,x);
		top = (int)jm_stack_top(double)(s);
        printf("pushed item %d=%d (stack top %g), vector size: %d, capacity: %d\n", i, x, jm_stack_top(double)(s), jm_vector_get_size(int)(v), jm_vector_reserve(int)(v,0));
		if(top != x) log_error("Stack top does not match the pushed value \n");
		if(jm_vector_get_size(int)(v) != VINIT_SIZE+i+1) log_error("Vector size %d is not as expected %d\n", jm_vector_get_size(int)(v), VINIT_SIZE+i+1);
    }
	{
		size_t index = jm_vector_find_index(int)(v, &k,jm_compare_int);
		k = TESTVAL;
		if( index != 2) log_error("Index of '%d' should be '2' but got %d\n", TESTVAL, k );
	}
    for( i = 0; i < 22; i++) {
        jm_stack_pop(double)(s);
    }
    jm_vector_resize_int(v,10);
    printf("Printing the vector \n");
    jm_vector_foreach_c(int)(v,print_int,0);
    jm_vector_qsort(int)(v, jm_compare_int);
    printf("Printing sorted vector \n");
    jm_vector_foreach_c(int)(v,print_int,0);
    printf("Index of %d is %d\n", k, jm_vector_bsearch_index(int)(v, &k, jm_compare_int));
    printf("Printing the stack \n");
    jm_stack_foreach(double)(s,print_dbl,0);

    jm_vector_free_data(int)(v);
    jm_stack_free(double)(s);
    return return_code;
}
