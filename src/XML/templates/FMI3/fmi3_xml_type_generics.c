#include "fmi3_xml_model_description_impl.h"
#include "fmi3_xml_type_impl.h"
#include "fmi3_xml_unit_impl.h"
#include "fmi3_xml_parser.h"


#define def_fmi3_xml_get_TYPE_min(TYPE, SIZE)                                                                   \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_min(fmi3_xml_##TYPE##_typedef_t* t) {                \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeMin.scalar##SIZE;                                                                     \
    }

#define def_fmi3_xml_get_TYPE_max(TYPE, SIZE)                                                                   \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_max(fmi3_xml_##TYPE##_typedef_t* t) {                \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeMax.scalar##SIZE;                                                                     \
    }

#define def_fmi3_xml_get_TYPE_nominal(TYPE, SIZE)                                                               \
    fmi3_##TYPE##SIZE##_t fmi3_xml_get_##TYPE##SIZE##_type_nominal(fmi3_xml_##TYPE##_typedef_t* t) {            \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeNominal.scalar##SIZE;                                                                 \
    }

#define def_fmi3_xml_get_TYPE_unit(TYPE, SIZE)                                                                  \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_unit(fmi3_xml_##TYPE##_typedef_t* t) {                    \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                       \
        if(du) return du->baseUnit;                                                                             \
        return 0;                                                                                               \
    }

#define def_fmi3_xml_get_TYPE_is_relative_quantity(TYPE, SIZE)                                                  \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_relative_quantity(fmi3_xml_##TYPE##_typedef_t* t) {                 \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeBase.isRelativeQuantity;                                                              \
    }

#define def_fmi3_xml_get_TYPE_is_unbounded(TYPE, SIZE)                                                          \
    int fmi3_xml_get_##TYPE##SIZE##_type_is_unbounded(fmi3_xml_##TYPE##_typedef_t* t) {                         \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)(vt->typeBase.baseTypeStruct); \
        return props->typeBase.isUnbounded;                                                                     \
    }

#define def_fmi3_xml_get_TYPE_display_unit(TYPE, SIZE)                                                          \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE##SIZE##_type_display_unit(fmi3_xml_##TYPE##_typedef_t* t) {    \
        fmi3_xml_variable_typedef_t* vt = (void*)t;                                                             \
        fmi3_xml_##TYPE##_type_props_t* props = (fmi3_xml_##TYPE##_type_props_t*)vt->typeBase.baseTypeStruct;   \
        fmi3_xml_display_unit_t* du = props->displayUnit;                                                       \
        if(!du || (&du->baseUnit->defaultDisplay == du)) return 0;                                              \
        return du;                                                                                              \
    }

#define def_fmi3_xml_get_TYPE_all(              TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_min(                  TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_max(                  TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_nominal(              TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_unit(                 TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_is_relative_quantity( TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_is_unbounded(         TYPE, SIZE) \
    def_fmi3_xml_get_TYPE_display_unit(         TYPE, SIZE)

def_fmi3_xml_get_TYPE_all(float, 64)
def_fmi3_xml_get_TYPE_all(float, 32)

