#include "../src/FMI3/fmi3_xml_model_description_impl.h"
#include "../src/FMI3/fmi3_xml_type_impl.h"
#include "../src/FMI3/fmi3_xml_unit_impl.h"
#include "../src/FMI3/fmi3_xml_parser.h"


#define gen_fmi3_xml_get_TYPE_min(TYPE, SIZE, SIGN)                                                             \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_min(fmi3_xml_##TYPE##_typedef_t* t) {                \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeMin.scalar##SIZE##SIGN;                                                               \
    }

#define gen_fmi3_xml_get_TYPE_max(TYPE, SIZE, SIGN)                                                             \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_max(fmi3_xml_##TYPE##_typedef_t* t) {                \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeMax.scalar##SIZE##SIGN;                                                                     \
    }

#define gen_fmi3_xml_get_TYPE_nominal(TYPE, SIZE, SIGN)                                                         \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_nominal(fmi3_xml_##TYPE##_typedef_t* t) {            \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeNominal.scalar##SIZE##SIGN;                                                                 \
    }

#define gen_fmi3_xml_get_TYPE_unit(TYPE, SIZE)                                                                  \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_unit(fmi3_xml_##TYPE##_typedef_t* t) {                    \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                       \
        if(du) return du->baseUnit;                                                                             \
        return 0;                                                                                               \
    }

#define gen_fmi3_xml_get_TYPE_is_relative_quantity(TYPE, SIZE)                                                  \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_relative_quantity(fmi3_xml_##TYPE##_typedef_t* t) {                 \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeBase.isRelativeQuantity;                                                              \
    }

#define gen_fmi3_xml_get_TYPE_is_unbounded(TYPE, SIZE)                                                          \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_unbounded(fmi3_xml_##TYPE##_typedef_t* t) {                         \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeBase.isUnbounded;                                                                     \
    }

#define gen_fmi3_xml_get_TYPE_display_unit(TYPE, SIZE)                                                          \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_display_unit(fmi3_xml_##TYPE##_typedef_t* t) {    \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)vt->typeBase.baseTypeStruct;   \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                       \
        if(!du || (&du->baseUnit->defaultDisplay == du)) return 0;                                              \
        return du;                                                                                              \
    }


#define SIGNED s
#define UNSIGNED u

/* float */
/*
    - adding a sign to float variable name to make the code reusable for int
    - SIGN is a param instead of hard-coded, since otherwise a wrapper macro will be required for expanding it
*/
#define gen_all_fmi3_xml_get_TYPE_XX_float(SIZE, SIGN)               \
    gen_fmi3_xml_get_TYPE_min(                  float, SIZE, SIGN)   \
    gen_fmi3_xml_get_TYPE_max(                  float, SIZE, SIGN)   \
    gen_fmi3_xml_get_TYPE_nominal(              float, SIZE, SIGN)   \
    gen_fmi3_xml_get_TYPE_unit(                 float, SIZE)         \
    gen_fmi3_xml_get_TYPE_is_relative_quantity( float, SIZE)         \
    gen_fmi3_xml_get_TYPE_is_unbounded(         float, SIZE)         \
    gen_fmi3_xml_get_TYPE_display_unit(         float, SIZE)

gen_all_fmi3_xml_get_TYPE_XX_float(64, SIGNED)
gen_all_fmi3_xml_get_TYPE_XX_float(32, SIGNED)

/* int */
#define gen_all_fmi3_xml_get_TYPE_XX_intXX(TYPE, SIZE, SIGN)        \
    gen_fmi3_xml_get_TYPE_min(                  TYPE, SIZE, SIGN)   \
    gen_fmi3_xml_get_TYPE_max(                  TYPE, SIZE, SIGN)

gen_all_fmi3_xml_get_TYPE_XX_intXX(int, 8, SIGNED)

