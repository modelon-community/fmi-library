#define gen_fmi3_import_get_variable_as_TYPE(TYPE)                                                   \
    fmi3_import_##TYPE##_variable_t* fmi3_import_get_variable_as_##TYPE(fmi3_import_variable_t* v) { \
        return fmi3_xml_get_variable_as_##TYPE(v);                                                   \
    }

#define gen_fmi3_import_get_TYPE_variable_ATTR(ATTR, RET_TYPE, TYPE)                                                         \
    RET_TYPE fmi3_import_get_##TYPE##_variable_##ATTR(fmi3_import_##TYPE##_variable_t* v) { \
        return fmi3_xml_get_##TYPE##_variable_##ATTR(v);                                                                     \
    }

#define gen_fmi3_import_get_TYPE_variable_derivative_of(     TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (derivative_of,     fmi3_import_##TYPE##_variable_t*, TYPE)
#define gen_fmi3_import_get_TYPE_variable_reinit(            TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (reinit,            fmi3_boolean_t,                   TYPE)
#define gen_fmi3_import_get_TYPE_variable_unit(              TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (unit,              fmi3_import_unit_t*,              TYPE)
#define gen_fmi3_import_get_TYPE_variable_display_unit(      TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (display_unit,      fmi3_import_display_unit_t*,      TYPE)
#define gen_fmi3_import_get_TYPE_variable_quantity(          TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (quantity,          fmi3_string_t,                    TYPE)
#define gen_fmi3_import_get_TYPE_variable_unbounded(         TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (unbounded,         fmi3_boolean_t,                   TYPE)
#define gen_fmi3_import_get_TYPE_variable_relative_quantity( TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (relative_quantity, fmi3_boolean_t,                   TYPE)
#define gen_fmi3_import_get_TYPE_variable_min(               TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (min,               fmi3_##TYPE##_t,                  TYPE)
#define gen_fmi3_import_get_TYPE_variable_max(               TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (max,               fmi3_##TYPE##_t,                  TYPE)
#define gen_fmi3_import_get_TYPE_variable_nominal(           TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (nominal,           fmi3_##TYPE##_t,                  TYPE)
#define gen_fmi3_import_get_TYPE_variable_start(             TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (start,             fmi3_##TYPE##_t,                  TYPE)
#define gen_fmi3_import_get_TYPE_variable_start_array(       TYPE) gen_fmi3_import_get_TYPE_variable_ATTR (start_array,       fmi3_##TYPE##_t*,                 TYPE)

#define gen_all_fmi3_import_get_floatXX_variable_ATTR(TYPE)    \
    gen_fmi3_import_get_variable_as_TYPE                (TYPE) \
    gen_fmi3_import_get_TYPE_variable_min               (TYPE) \
    gen_fmi3_import_get_TYPE_variable_max               (TYPE) \
    gen_fmi3_import_get_TYPE_variable_nominal           (TYPE) \
    gen_fmi3_import_get_TYPE_variable_start             (TYPE) \
    gen_fmi3_import_get_TYPE_variable_start_array       (TYPE) \
    gen_fmi3_import_get_TYPE_variable_reinit            (TYPE) \
    gen_fmi3_import_get_TYPE_variable_derivative_of     (TYPE) \
    gen_fmi3_import_get_TYPE_variable_unit              (TYPE) \
    gen_fmi3_import_get_TYPE_variable_display_unit      (TYPE) \
    gen_fmi3_import_get_TYPE_variable_quantity          (TYPE) \
    gen_fmi3_import_get_TYPE_variable_unbounded         (TYPE) \
    gen_fmi3_import_get_TYPE_variable_relative_quantity (TYPE)

gen_all_fmi3_import_get_floatXX_variable_ATTR(float64)
gen_all_fmi3_import_get_floatXX_variable_ATTR(float32)

#define gen_all_fmi3_import_get_intXX_variable_ATTR(TYPE) \
    gen_fmi3_import_get_variable_as_TYPE       (TYPE)     \
    gen_fmi3_import_get_TYPE_variable_min      (TYPE)     \
    gen_fmi3_import_get_TYPE_variable_max      (TYPE)     \
    gen_fmi3_import_get_TYPE_variable_start    (TYPE)     \
    gen_fmi3_import_get_TYPE_variable_quantity (TYPE)

gen_all_fmi3_import_get_intXX_variable_ATTR(int64)
gen_all_fmi3_import_get_intXX_variable_ATTR(int32)
gen_all_fmi3_import_get_intXX_variable_ATTR(int16)
gen_all_fmi3_import_get_intXX_variable_ATTR(int8)
gen_all_fmi3_import_get_intXX_variable_ATTR(uint64)
gen_all_fmi3_import_get_intXX_variable_ATTR(uint32)
gen_all_fmi3_import_get_intXX_variable_ATTR(uint16)
gen_all_fmi3_import_get_intXX_variable_ATTR(uint8)
