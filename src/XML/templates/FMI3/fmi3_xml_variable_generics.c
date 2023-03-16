#define gen_fmi3_xml_get_variable_as_TYPEXX(TYPEXX)                                                 \
    fmi3_xml_##TYPEXX##_variable_t* fmi3_xml_get_variable_as_##TYPEXX(fmi3_xml_variable_t* v) {     \
        if (fmi3_xml_get_variable_base_type(v) == fmi3_base_type_##TYPEXX)                          \
            return (void*)v;                                                                        \
        return 0;                                                                                   \
    }

#define gen_fmi3_xml_get_floatXX_variable_derivative_of(TYPE)                                                       \
    fmi3_xml_##TYPE##_variable_t* fmi3_xml_get_##TYPE##_variable_derivative_of(fmi3_xml_##TYPE##_variable_t* v) {   \
        fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                                         \
                                                                                                                    \
        return (fmi3_xml_##TYPE##_variable_t *)vv->derivativeOf;                                                    \
    }                                                                                                               

#define gen_fmi3_xml_get_floatXX_variable_reinit(TYPE)                                      \
    fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_reinit(fmi3_xml_##TYPE##_variable_t* v) { \
        fmi3_xml_variable_t *vv = (fmi3_xml_variable_t *)v;                                 \
        return (fmi3_boolean_t)vv->reinit;                                                  \
    }                                                                                           

#define gen_fmi3_xml_get_floatXX_variable_relative_quantity(TYPE)                                            \
    fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_relative_quantity(fmi3_xml_##TYPE##_variable_t* v) {       \
        fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v); \
        assert(props);                                                                                       \
        return props->super.isRelativeQuantity;                                                              \
    }

#define gen_fmi3_xml_get_floatXX_variable_unbounded(TYPE)                                                    \
    fmi3_boolean_t fmi3_xml_get_##TYPE##_variable_unbounded(fmi3_xml_##TYPE##_variable_t* v) {               \
        fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v); \
        assert(props);                                                                                       \
        return props->super.isUnbounded;                                                                     \
    }

#define gen_fmi3_xml_get_floatXX_variable_quantity(TYPE)                                                     \
    fmi3_string_t fmi3_xml_get_##TYPE##_variable_quantity(fmi3_xml_##TYPE##_variable_t* v) {                 \
        fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v); \
        if(!props) return NULL;                                                                              \
        return (fmi3_string_t)props->quantity;                                                               \
    }

#define gen_fmi3_xml_get_floatXX_variable_unit(TYPE)                                                         \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE##_variable_unit(fmi3_xml_##TYPE##_variable_t* v) {                  \
        fmi3_xml_float_type_props_t* props = fmi3_xml_find_type_struct_float_props((fmi3_xml_variable_t*)v); \
        if(!props || !props->displayUnit) return 0;                                                          \
        return props->displayUnit->baseUnit;                                                                 \
    }                                                                                                                           

#define gen_fmi3_xml_get_floatXX_variable_display_unit(TYPE)                                                            \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##_variable_display_unit(fmi3_xml_##TYPE##_variable_t* v) {             \
        fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                              \
        fmi3_xml_float_type_props_t* props =                                                                            \
            (fmi3_xml_float_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));        \
        if(!props || !props->displayUnit || !props->displayUnit->displayUnit[0]) return 0;                              \
        return props->displayUnit;                                                                                      \
    }                                                                                                                       

/** 
 * Function macro for generating wrappers for specific types.
 *   ATTR: function to generate, must be prefixed with '_' to avoid macro conflicts with 'min' and 'max'
 *   TYPE: float32, float64
 */
#define gen_fmi3_xml_get_floatXX_variable_ATTR(XX, TYPE, FIELD)                             \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable##XX(fmi3_xml_##TYPE##_variable_t* v) {   \
        return fmi3_xml_get_float_variable##XX((fmi3_xml_float_variable_t*)v).FIELD;        \
    }

#define gen_fmi3_xml_get_floatXX_variable_start_array(TYPE, FIELD)                                  \
    fmi3_##TYPE##_t* fmi3_xml_get_##TYPE##_variable_start_array(fmi3_xml_##TYPE##_variable_t* v) {  \
        return fmi3_xml_get_float_variable_start((fmi3_xml_float_variable_t*)v).FIELD;              \
    }


#define SIGNED s
#define UNSIGNED u

/* this macro is needed because otherwise the SIGN part won't get expanded */
#define expand_field(DIM_TYPE, SIZE, SIGN) DIM_TYPE##SIZE##SIGN

#define gen_all_fmi3_xml_get_float_variable_ATTR(TYPE, SIZE, SIGN)                           \
    gen_fmi3_xml_get_variable_as_TYPEXX                 (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_derivative_of     (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_reinit            (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_unit              (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_display_unit      (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_quantity          (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_unbounded         (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_relative_quantity (TYPE)                               \
    gen_fmi3_xml_get_floatXX_variable_ATTR(_min,     TYPE, expand_field(scalar, SIZE, SIGN)) \
    gen_fmi3_xml_get_floatXX_variable_ATTR(_max,     TYPE, expand_field(scalar, SIZE, SIGN)) \
    gen_fmi3_xml_get_floatXX_variable_ATTR(_nominal, TYPE, expand_field(scalar, SIZE, SIGN)) \
    gen_fmi3_xml_get_floatXX_variable_ATTR(_start,   TYPE, expand_field(scalar, SIZE, SIGN)) \
    gen_fmi3_xml_get_floatXX_variable_start_array(   TYPE, expand_field(array,  SIZE, SIGN))

gen_all_fmi3_xml_get_float_variable_ATTR(float64, 64, SIGNED)
gen_all_fmi3_xml_get_float_variable_ATTR(float32, 32, SIGNED)

/** 
 * Function macro for generating wrappers for specific types.
 *   ATTR: function to generate, must be prefixed with '_' to avoid macro conflicts with 'min' and 'max'
 */
#define gen_fmi3_xml_get_intXX_variable_ATTR(XX, TYPE, FIELD)                               \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable##XX(fmi3_xml_##TYPE##_variable_t* v) {   \
        return fmi3_xml_get_int_variable##XX((fmi3_xml_int_variable_t*)v).FIELD;            \
    }

#define gen_fmi3_xml_get_intXX_variable_start(TYPE, FIELD)                                                  \
    fmi3_##TYPE##_t fmi3_xml_get_##TYPE##_variable_start(fmi3_xml_##TYPE##_variable_t* v) {                 \
        fmi3_int_union_t start;                                                                             \
        return fmi3_xml_get_int_variable_start((fmi3_xml_int_variable_t*)v, &start) ? 0 : start.FIELD;      \
    }

#define gen_fmi3_xml_get_intXX_variable_quantity(TYPE)                                                                 \
    fmi3_string_t fmi3_xml_get_##TYPE##_variable_quantity(fmi3_xml_##TYPE##_variable_t* v) {                           \
        fmi3_xml_variable_t* vv = (fmi3_xml_variable_t*)v;                                                             \
        fmi3_xml_int_type_props_t* props =                                                                             \
                (fmi3_xml_int_type_props_t*)(fmi3_xml_find_type_struct(vv->type, fmi3_xml_type_struct_enu_props));     \
        if(!props) return NULL;                                                                                        \
        return (fmi3_string_t)props->quantity;                                                                         \
    }

#define gen_all_fmi3_xml_get_intXX_variable(TYPE, SIZE, SIGN)                                  \
    gen_fmi3_xml_get_variable_as_TYPEXX(TYPE)                                                  \
    gen_fmi3_xml_get_intXX_variable_ATTR     (_min,   TYPE, expand_field(scalar, SIZE, SIGN))  \
    gen_fmi3_xml_get_intXX_variable_ATTR     (_max,   TYPE, expand_field(scalar, SIZE, SIGN))  \
    gen_fmi3_xml_get_intXX_variable_start    (        TYPE, expand_field(scalar, SIZE, SIGN))  \
    gen_fmi3_xml_get_intXX_variable_quantity (        TYPE)

gen_all_fmi3_xml_get_intXX_variable(int64,  64,   SIGNED)
gen_all_fmi3_xml_get_intXX_variable(int32,  32,   SIGNED)
gen_all_fmi3_xml_get_intXX_variable(int16,  16,   SIGNED)
gen_all_fmi3_xml_get_intXX_variable(int8,    8,   SIGNED)
gen_all_fmi3_xml_get_intXX_variable(uint64, 64, UNSIGNED)
gen_all_fmi3_xml_get_intXX_variable(uint32, 32, UNSIGNED)
gen_all_fmi3_xml_get_intXX_variable(uint16, 16, UNSIGNED)
gen_all_fmi3_xml_get_intXX_variable(uint8,   8, UNSIGNED)
