#include "FMI3/fmi3_xml_model_description.h"


#define gen_fmi3_xml_get_TYPE_XX(XX, TYPE, TYPE_SIZE) \
    fmi3_##TYPE_SIZE##_t fmi3_xml_get_##TYPE_SIZE##_type##XX(fmi3_xml_##TYPE##_typedef_t*);

#define gen_fmi3_xml_get_TYPE_unit(TYPE, TYPE_SIZE) \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE_SIZE##_type_unit(fmi3_xml_##TYPE##_typedef_t*);

#define gen_fmi3_xml_get_TYPE_is_relative_quantity(TYPE, TYPE_SIZE) \
    int fmi3_xml_get_##TYPE_SIZE##_type_is_relative_quantity(fmi3_xml_##TYPE##_typedef_t*);

#define gen_fmi3_xml_get_TYPE_is_unbounded(TYPE, TYPE_SIZE) \
    int fmi3_xml_get_##TYPE_SIZE##_type_is_unbounded(fmi3_xml_##TYPE##_typedef_t*);

#define gen_fmi3_xml_get_TYPE_display_unit(TYPE, TYPE_SIZE) \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE_SIZE##_type_display_unit(fmi3_xml_##TYPE##_typedef_t*);

#define gen_all_fmi3_xml_get_TYPE_XX(TYPE, SIZE)                                \
    gen_fmi3_xml_get_TYPE_XX(                   _min,       TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_XX(                   _max,       TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_XX(                   _nominal,   TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_unit(                             TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_is_relative_quantity(             TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_is_unbounded(                     TYPE, TYPE##SIZE)   \
    gen_fmi3_xml_get_TYPE_display_unit(                     TYPE, TYPE##SIZE)

gen_all_fmi3_xml_get_TYPE_XX(float, 64)
gen_all_fmi3_xml_get_TYPE_XX(float, 32)
