#include "../src/FMI3/fmi3_import_impl.h"

#define GEN_FUNC_import_get_type_XX(XX, TYPE, TYPE_SIZE)                                                    \
    fmi3_##TYPE_SIZE##_t fmi3_import_get_##TYPE_SIZE##_type##XX(fmi3_import_##TYPE##_typedef_t* t) {        \
        return fmi3_xml_get_##TYPE_SIZE##_type##XX(t);                                                      \
    }

#define GEN_FUNC_import_get_type_unit(TYPE, TYPE_SIZE)                                                      \
    fmi3_import_unit_t* fmi3_import_get_##TYPE_SIZE##_type_unit(fmi3_import_##TYPE##_typedef_t* t) {        \
        return fmi3_xml_get_##TYPE_SIZE##_type_unit(t);                                                      \
    }


#define GEN_FUNC_import_get_type_is_relative_quantity(TYPE, TYPE_SIZE)                                  \
    int fmi3_import_get_##TYPE_SIZE##_type_is_relative_quantity(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_is_relative_quantity(t);                                 \
    }

#define GEN_FUNC_import_get_type_is_unbounded(TYPE, TYPE_SIZE)                              \
    int fmi3_import_get_##TYPE_SIZE##_type_is_unbounded(fmi3_import_##TYPE##_typedef_t* t) { \
        return fmi3_xml_get_##TYPE_SIZE##_type_is_unbounded(t);                             \
    }

#define GEN_FUNC_import_get_type_display_unit(TYPE, TYPE_SIZE)                                                          \
    fmi3_import_display_unit_t* fmi3_import_get_##TYPE_SIZE##_type_display_unit(fmi3_import_##TYPE##_typedef_t* t) {    \
        return fmi3_xml_get_##TYPE_SIZE##_type_display_unit(t);                                                         \
    }



#define GEN_FUNC_import_get_type_XX_all(TYPE_SIZE)                  \
GEN_FUNC_import_get_type_XX(_min,               float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_XX(_max,               float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_XX(_nominal,           float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_unit(                  float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_is_relative_quantity(  float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_is_unbounded(          float, TYPE_SIZE)   \
GEN_FUNC_import_get_type_display_unit(          float, TYPE_SIZE)

GEN_FUNC_import_get_type_XX_all(float64)
GEN_FUNC_import_get_type_XX_all(float32)

