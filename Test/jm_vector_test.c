#include <stdio.h>
#include <stdlib.h>

#include <jm_vector.h>
#include <jm_stack.h>

void print_int(int i,void* data) {
    printf("%d\n", i);
}

void print_dbl(double d,void* data) {
    printf("%g\n", d);
}

int compar_int(int* a, int* b) {
    return (*a - *b);
}

int main() {
    int i, k;
    jm_vector(int) stackv;
    jm_stack(double)* s;

    jm_vector(int)* v = &stackv;
    srand(0);
    s = jm_stack_alloc(double)(100, 0 );
    jm_vector_init(int)(v,5,0);
    jm_vector_zero(int)(v);
    jm_vector_set_item_int(v, 2, 22);
    for( i = 0; i < 32; i++) {
        int x = rand();
        jm_vector_push_back(int)(v,x);
        jm_stack_push(double)(s,x);
        printf("pushed item %d=%d (stack top %g), vector size: %d, capacity: %d\n", i, x, jm_stack_top(double)(s), jm_vector_get_size(int)(v), jm_vector_reserve(int)(v,0));
    }
    k = 22;
    printf("Index of %d is %d\n", k, jm_vector_find_index(int)(v, &k,jm_compare_int));
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
    return 0;
}
