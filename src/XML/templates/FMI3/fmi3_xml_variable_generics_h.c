#define gen_fmi3_xml_get_variable_as_TYPE_h(TYPE)                                        \
    fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_variable_as_##TYPE(fmi3_xml_variable_t*);

#define gen_all_fmi3_xml_get_floatXX_variable_ATTR(TYPE)                                                              \
    gen_fmi3_xml_get_variable_as_TYPE_h(TYPE)                                                                         \
    fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_##TYPE##_variable_derivative_of     (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_boolean_t                fmi3_xml_get_##TYPE##_variable_reinit            (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_xml_unit_t*              fmi3_xml_get_##TYPE##_variable_unit              (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_xml_display_unit_t*      fmi3_xml_get_##TYPE##_variable_display_unit      (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t               fmi3_xml_get_##TYPE##_variable_start             (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t               fmi3_xml_get_##TYPE##_variable_min               (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t               fmi3_xml_get_##TYPE##_variable_max               (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t               fmi3_xml_get_##TYPE##_variable_nominal           (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_string_t                 fmi3_xml_get_##TYPE##_variable_quantity          (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_boolean_t                fmi3_xml_get_##TYPE##_variable_unbounded         (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_boolean_t                fmi3_xml_get_##TYPE##_variable_relative_quantity (fmi3_xml_##TYPE##_variable_t* v);

gen_all_fmi3_xml_get_floatXX_variable_ATTR(float64)
gen_all_fmi3_xml_get_floatXX_variable_ATTR(float32)

#define gen_all_fmi3_xml_get_intXX_variable_ATTR(TYPE)                                         \
    gen_fmi3_xml_get_variable_as_TYPE_h(TYPE)                                                  \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_min      (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_max      (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_start    (fmi3_xml_##TYPE##_variable_t* v); \
    fmi3_string_t   fmi3_xml_get_##TYPE##_variable_quantity (fmi3_xml_##TYPE##_variable_t* v);

gen_all_fmi3_xml_get_intXX_variable_ATTR(int64)
gen_all_fmi3_xml_get_intXX_variable_ATTR(int32)
gen_all_fmi3_xml_get_intXX_variable_ATTR(int16)
gen_all_fmi3_xml_get_intXX_variable_ATTR(int8)
gen_all_fmi3_xml_get_intXX_variable_ATTR(uint64)
gen_all_fmi3_xml_get_intXX_variable_ATTR(uint32)
gen_all_fmi3_xml_get_intXX_variable_ATTR(uint16)
gen_all_fmi3_xml_get_intXX_variable_ATTR(uint8)
