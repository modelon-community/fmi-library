/* TYPE_VIRTUAL: the type without any size or sign */
#define gen_fmi3_xml_get_TYPE_type_ATTR(ATTR, RET_TYPE, TYPE, TYPE_VIRTUAL)    \
    RET_TYPE fmi3_xml_get_##TYPE##_type_##ATTR(fmi3_xml_##TYPE_VIRTUAL##_typedef_t*);

#define gen_fmi3_xml_get_TYPE_type_min(                  TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(min,                  fmi3_##TYPE##_t,          TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_max(                  TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(max,                  fmi3_##TYPE##_t,          TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_nominal(              TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(nominal,              fmi3_##TYPE##_t,          TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_unit(                 TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(unit,                 fmi3_xml_unit_t*,         TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_is_relative_quantity( TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(is_relative_quantity, int,                      TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_is_unbounded(         TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(is_unbounded,         int,                      TYPE, TYPE_VIRTUAL)
#define gen_fmi3_xml_get_TYPE_type_display_unit(         TYPE, TYPE_VIRTUAL) gen_fmi3_xml_get_TYPE_type_ATTR(display_unit,         fmi3_xml_display_unit_t*, TYPE, TYPE_VIRTUAL)

/* float */
#define gen_all_fmi3_xml_get_TYPE_ATTR_float(SIZE)                         \
    gen_fmi3_xml_get_TYPE_type_min                  (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_max                  (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_nominal              (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_unit                 (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_is_relative_quantity (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_is_unbounded         (float##SIZE, float)   \
    gen_fmi3_xml_get_TYPE_type_display_unit         (float##SIZE, float)

gen_all_fmi3_xml_get_TYPE_ATTR_float(64)
gen_all_fmi3_xml_get_TYPE_ATTR_float(32)

/* int */
#define gen_all_fmi3_xml_get_TYPE_ATTR_intXX(SIZE)  \
    gen_fmi3_xml_get_TYPE_type_min (int##SIZE, int) \
    gen_fmi3_xml_get_TYPE_type_max (int##SIZE, int)

#define gen_all_fmi3_xml_get_TYPE_ATTR_uintXX(SIZE)  \
    gen_fmi3_xml_get_TYPE_type_min (uint##SIZE, int) \
    gen_fmi3_xml_get_TYPE_type_max (uint##SIZE, int)

gen_all_fmi3_xml_get_TYPE_ATTR_intXX(64)
gen_all_fmi3_xml_get_TYPE_ATTR_intXX(32)
gen_all_fmi3_xml_get_TYPE_ATTR_intXX(16)
gen_all_fmi3_xml_get_TYPE_ATTR_intXX(8)
gen_all_fmi3_xml_get_TYPE_ATTR_uintXX(64)
gen_all_fmi3_xml_get_TYPE_ATTR_uintXX(32)
gen_all_fmi3_xml_get_TYPE_ATTR_uintXX(16)
gen_all_fmi3_xml_get_TYPE_ATTR_uintXX(8)
