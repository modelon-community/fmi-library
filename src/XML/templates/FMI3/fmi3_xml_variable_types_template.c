#define def_get_TYPE_variable_derivative_of(TYPE)                                                                \
fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_##TYPE##_variable_derivative_of(fmi3_xml_##TYPE##_variable_t* v) {    \
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                                          \
                                                                                                                 \
    return (fmi3_xml_##TYPE##_variable_t *)vv->derivativeOf;                                                     \
}                                                                                                               

#define def_get_TYPE_variable_reinit(TYPE)                                                   \
fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_reinit(fmi3_xml_##TYPE##_variable_t* v) {      \
    fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                      \
    return (fmi3_boolean_t)vv->reinit;                                                       \
}                                                                                           

#define def_get_TYPE_variable_unit(TYPE)                                                                                     \
fmi3_xml_unit_t* fmi3_xml_get_##TYPE##_variable_unit(fmi3_xml_##TYPE##_variable_t* v) {                                      \
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                                       \
    fmi3_xml_float_type_props_t* props =                                                                                     \
            (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));         \
    if(!props || !props->displayUnit) return 0;                                                                              \
    return props->displayUnit->baseUnit;                                                                                     \
}                                                                                                                           

#define def_get_TYPE_variable_display_unit(TYPE)                                                                         \
fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##_variable_display_unit(fmi3_xml_##TYPE##_variable_t* v) {                  \
    fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                                   \
    fmi3_xml_float_type_props_t* props =                                                                                 \
        (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->typeBase, fmi3_xml_type_struct_enu_props));         \
    if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;                                   \
    return props->displayUnit;                                                                                           \
}                                                                                                                       

/** 
 * Function macro for generating wrappers for specific types.
 *   XX: function to generate, must be prefixed with '_' to avoid macro conflicts with 'min' and 'max'
 *   TYPE: float32, float64
 */
#define COMMENT(X) __COMMENT_START__ X __COMMENT_END__
#define def_get_float_variable_XX(XX, TYPE, FIELD)                                      \
fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable##XX(fmi3_xml_##TYPE##_variable_t* v) {   \
    return fmi3_xml_get_float_variable##XX((fmi3_xml_float_variable_t*)v).FIELD;        \
}                                                                                          

#define def_get_float_variable_XX_array(XX, TYPE, FIELD)                                        \
fmi3_##TYPE##_t* fmi3_xml_get_##TYPE##_variable##XX##_array(fmi3_xml_##TYPE##_variable_t* v) {  \
    return fmi3_xml_get_float_variable##XX((fmi3_xml_float_variable_t*)v).FIELD;                \
}                                                                                                  

#define def_get_TYPE_variable_define_all_functions(TYPE, SIZE)  \
COMMENT(TYPE)                                                   \
def_get_TYPE_variable_derivative_of(TYPE)                       \
def_get_TYPE_variable_reinit(TYPE)                              \
def_get_TYPE_variable_unit(TYPE)                                \
def_get_TYPE_variable_display_unit(TYPE)                        \
def_get_float_variable_XX(_min, TYPE, scalar##SIZE)             \
def_get_float_variable_XX(_max, TYPE, scalar##SIZE)             \
def_get_float_variable_XX(_nominal, TYPE, scalar##SIZE)         \
def_get_float_variable_XX(_start, TYPE, scalar##SIZE)           \
def_get_float_variable_XX_array(_start, TYPE, array##SIZE)

def_get_TYPE_variable_define_all_functions(float64, 64)
def_get_TYPE_variable_define_all_functions(float32, 32)

