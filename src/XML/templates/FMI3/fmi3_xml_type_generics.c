#define gen_inline_get_props(TYPE_NO_SIGN)                                                                                  \
    fmi3_xml_variable_typedef_t* vt = (void*)t;                                                                             \
    fmi3_xml_##TYPE_NO_SIGN##_type_props_t* props = (fmi3_xml_##TYPE_NO_SIGN##_type_props_t*)(vt->super.nextLayer);

#define gen_fmi3_xml_get_TYPE_type_min(TYPE, SIZE, SIGN, TYPE_NO_SIGN)                                          \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_min(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {        \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        return props->typeMin.scalar##SIZE##SIGN;                                                               \
    }

#define gen_fmi3_xml_get_TYPE_type_max(TYPE, SIZE, SIGN, TYPE_NO_SIGN)                                          \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_max(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {        \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        return props->typeMax.scalar##SIZE##SIGN;                                                               \
    }

#define gen_fmi3_xml_get_TYPE_type_nominal(TYPE, SIZE, SIGN, TYPE_NO_SIGN)                                      \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_nominal(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {    \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        return props->typeNominal.scalar##SIZE##SIGN;                                                           \
    }

#define gen_fmi3_xml_get_TYPE_type_unit(TYPE, SIZE, TYPE_NO_SIGN)                                               \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_unit(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {            \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                       \
        if(du) return du->baseUnit;                                                                             \
        return 0;                                                                                               \
    }

#define gen_fmi3_xml_get_TYPE_type_is_relative_quantity(TYPE, SIZE, TYPE_NO_SIGN)                               \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_relative_quantity(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {         \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        return props->super.isRelativeQuantity;                                                                 \
    }

#define gen_fmi3_xml_get_TYPE_type_is_unbounded(TYPE, SIZE, TYPE_NO_SIGN)                                       \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_unbounded(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {                 \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                      \
        return props->super.isUnbounded;                                                                        \
    }

#define gen_fmi3_xml_get_TYPE_type_display_unit(TYPE, SIZE, TYPE_NO_SIGN)                                               \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_display_unit(fmi3_xml_##TYPE_NO_SIGN##_typedef_t* t) {    \
        gen_inline_get_props(TYPE_NO_SIGN)                                                                              \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                               \
        if(!du || (&du->baseUnit->defaultDisplay == du)) return 0;                                                      \
        return du;                                                                                                      \
    }

#define SIGNED s
#define UNSIGNED u

/* float */
/*
    - adding a sign to float variable name to make the code reusable for int
    - SIGN is a param instead of hard-coded, since otherwise a wrapper macro will be required for expanding it
*/
#define gen_all_fmi3_xml_get_TYPE_type_XX_float(SIZE, SIGN)                      \
    gen_fmi3_xml_get_TYPE_type_min                  (float, SIZE, SIGN, float)   \
    gen_fmi3_xml_get_TYPE_type_max                  (float, SIZE, SIGN, float)   \
    gen_fmi3_xml_get_TYPE_type_nominal              (float, SIZE, SIGN, float)   \
    gen_fmi3_xml_get_TYPE_type_unit                 (float, SIZE, float)         \
    gen_fmi3_xml_get_TYPE_type_is_relative_quantity (float, SIZE, float)         \
    gen_fmi3_xml_get_TYPE_type_is_unbounded         (float, SIZE, float)         \
    gen_fmi3_xml_get_TYPE_type_display_unit         (float, SIZE, float)

gen_all_fmi3_xml_get_TYPE_type_XX_float(64, SIGNED)
gen_all_fmi3_xml_get_TYPE_type_XX_float(32, SIGNED)

/* int */
#define gen_all_fmi3_xml_get_TYPE_type_XX_intXX(TYPE, SIZE, SIGN, TYPE_NO_SIGN) \
    gen_fmi3_xml_get_TYPE_type_min (TYPE, SIZE, SIGN, TYPE_NO_SIGN)             \
    gen_fmi3_xml_get_TYPE_type_max (TYPE, SIZE, SIGN, TYPE_NO_SIGN)

gen_all_fmi3_xml_get_TYPE_type_XX_intXX(int,  64,  SIGNED,   int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(int,  32,  SIGNED,   int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(int,  16,  SIGNED,   int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(int,  8,   SIGNED,   int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(uint, 64,  UNSIGNED, int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(uint, 32,  UNSIGNED, int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(uint, 16,  UNSIGNED, int)
gen_all_fmi3_xml_get_TYPE_type_XX_intXX(uint, 8,   UNSIGNED, int)

