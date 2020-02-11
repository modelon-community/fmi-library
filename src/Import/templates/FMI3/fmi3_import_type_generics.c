#include "../src/FMI3/fmi3_import_impl.h"

#define gen_fmi3_import_get_TYPE_XX(XX, TYPE, TYPE_SIZE)                                                \
    fmi3_##TYPE_SIZE##_t fmi3_import_get_##TYPE_SIZE##_type##XX(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type##XX(t);                                                  \
    }

#define gen_fmi3_import_get_type_unit(TYPE, TYPE_SIZE)                                                  \
    fmi3_import_unit_t* fmi3_import_get_##TYPE_SIZE##_type_unit(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_unit(t);                                                 \
    }


#define gen_fmi3_import_get_type_is_relative_quantity(TYPE, TYPE_SIZE)                                  \
    int fmi3_import_get_##TYPE_SIZE##_type_is_relative_quantity(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_is_relative_quantity(t);                                 \
    }

#define gen_fmi3_import_get_type_is_unbounded(TYPE, TYPE_SIZE)                                  \
    int fmi3_import_get_##TYPE_SIZE##_type_is_unbounded(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_is_unbounded(t);                                 \
    }

#define gen_fmi3_import_get_type_display_unit(TYPE, TYPE_SIZE)                                                          \
    fmi3_import_display_unit_t* fmi3_import_get_##TYPE_SIZE##_type_display_unit(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_display_unit(t);                                                         \
    }



#define gen_all_fmi3_import_get_TYPE_XX(TYPE_SIZE)                      \
    gen_fmi3_import_get_TYPE_XX(_min,               float, TYPE_SIZE)   \
    gen_fmi3_import_get_TYPE_XX(_max,               float, TYPE_SIZE)   \
    gen_fmi3_import_get_TYPE_XX(_nominal,           float, TYPE_SIZE)   \
    gen_fmi3_import_get_type_unit(                  float, TYPE_SIZE)   \
    gen_fmi3_import_get_type_is_relative_quantity(  float, TYPE_SIZE)   \
    gen_fmi3_import_get_type_is_unbounded(          float, TYPE_SIZE)   \
    gen_fmi3_import_get_type_display_unit(          float, TYPE_SIZE)

gen_all_fmi3_import_get_TYPE_XX(float64)
gen_all_fmi3_import_get_TYPE_XX(float32)

