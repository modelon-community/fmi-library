#define COMMENT(X) __COMMENT_START__ X __COMMENT_END__

#define def_fmi3_xml_get_TYPE_variable_XX(TYPE)                                                                 \
fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_##TYPE##_variable_derivative_of(fmi3_xml_##TYPE##_variable_t* v);    \
fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_reinit(fmi3_xml_##TYPE##_variable_t* v);                          \
fmi3_xml_unit_t* fmi3_xml_get_##TYPE##_variable_unit(fmi3_xml_##TYPE##_variable_t* v);                          \
fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##_variable_display_unit(fmi3_xml_##TYPE##_variable_t* v);          \
fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_start (fmi3_xml_##TYPE##_variable_t* v);                         \
fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_min (fmi3_xml_##TYPE##_variable_t* v);                           \
fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_max (fmi3_xml_##TYPE##_variable_t* v);                           \
fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_nominal (fmi3_xml_##TYPE##_variable_t* v);

COMMENT(float64)
def_fmi3_xml_get_TYPE_variable_XX(float64)
COMMENT(float32)
def_fmi3_xml_get_TYPE_variable_XX(float32)

