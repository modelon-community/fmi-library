#include "FMI3/fmi3_import_variable.h"
#include "../src/FMI3/fmi3_import_impl.h"
#include "../src/FMI3/fmi3_import_variable_list_impl.h"
#include "FMI3/fmi3_import_dimension.h"
#include "FMI3/fmi3_import_dimension_list.h"


#define COMMENT(X) __COMMENT_START__ X __COMMENT_END__

#define gen_fmi3_import_get_TYPE_variable_derivative_of(TYPE)                                                               \
    fmi3_import_##TYPE##_variable_t* fmi3_import_get_##TYPE##_variable_derivative_of(fmi3_import_##TYPE##_variable_t* v) {  \
        return fmi3_xml_get_##TYPE##_variable_derivative_of(v);                                                             \
    }


#define gen_fmi3_import_get_TYPE_variable_reinit(TYPE)                                              \
    fmi3_boolean_t fmi3_import_get_##TYPE##_variable_reinit(fmi3_import_##TYPE##_variable_t* v) {   \
        return fmi3_xml_get_##TYPE##_variable_reinit(v);                                            \
    }

#define gen_fmi3_import_get_TYPE_variable_unit(TYPE)                                                    \
    fmi3_import_unit_t* fmi3_import_get_##TYPE##_variable_unit(fmi3_import_##TYPE##_variable_t* v) {    \
        return fmi3_xml_get_##TYPE##_variable_unit(v);                                                  \
    }

#define gen_fmi3_import_get_TYPE_variable_display_unit(TYPE)                                                        \
    fmi3_import_display_unit_t* fmi3_import_get_##TYPE##_variable_display_unit(fmi3_import_##TYPE##_variable_t* v){ \
        return fmi3_xml_get_##TYPE##_variable_display_unit(v);                                                      \
    }

#define gen_fmi3_import_get_variable_XX(XX, TYPE)                                                \
    fmi3_##TYPE##_t fmi3_import_get_##TYPE##_variable##XX(fmi3_import_##TYPE##_variable_t* v) {  \
        return fmi3_xml_get_##TYPE##_variable##XX(v);                                            \
    }                                                                                           

#define gen_fmi3_import_get_variable_XX_ptr(XX, TYPE)                                               \
    fmi3_##TYPE##_t* fmi3_import_get_##TYPE##_variable##XX(fmi3_import_##TYPE##_variable_t* v) {    \
        return fmi3_xml_get_##TYPE##_variable##XX(v);                                               \
    }                                                                                            

#define gen_all_fmi3_import_get_TYPE_variable_XX(TYPE_SIZE)          \
    COMMENT(TYPE_SIZE)                                               \
    gen_fmi3_import_get_variable_XX(_min,                TYPE_SIZE)  \
    gen_fmi3_import_get_variable_XX(_max,                TYPE_SIZE)  \
    gen_fmi3_import_get_variable_XX(_nominal,            TYPE_SIZE)  \
    gen_fmi3_import_get_variable_XX(_start,              TYPE_SIZE)  \
    gen_fmi3_import_get_variable_XX_ptr(_start_array,    TYPE_SIZE)  \
    gen_fmi3_import_get_TYPE_variable_reinit(            TYPE_SIZE)  \
    gen_fmi3_import_get_TYPE_variable_derivative_of(     TYPE_SIZE)  \
    gen_fmi3_import_get_TYPE_variable_unit(              TYPE_SIZE)  \
    gen_fmi3_import_get_TYPE_variable_display_unit(      TYPE_SIZE)

gen_all_fmi3_import_get_TYPE_variable_XX(float64)
gen_all_fmi3_import_get_TYPE_variable_XX(float32)

