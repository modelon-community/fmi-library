#include "FMI3/fmi3_xml_model_description.h"


#define def_fmi3_xml_get_type_XX(XX, TYPE, TYPE_SIZE) \
    fmi3_##TYPE_SIZE##_t fmi3_xml_get_##TYPE_SIZE##_type##XX(fmi3_xml_##TYPE##_typedef_t*);

#define def_fmi3_xml_get_type_unit(TYPE, TYPE_SIZE) \
    fmi3_xml_unit_t* fmi3_xml_get_##TYPE_SIZE##_type_unit(fmi3_xml_##TYPE##_typedef_t*);

#define def_fmi3_xml_get_type_is_relative_quantity(TYPE, TYPE_SIZE) \
    int fmi3_xml_get_##TYPE_SIZE##_type_is_relative_quantity(fmi3_xml_##TYPE##_typedef_t*);

#define def_fmi3_xml_get_type_is_unbounded(TYPE, TYPE_SIZE) \
    int fmi3_xml_get_##TYPE_SIZE##_type_is_unbounded(fmi3_xml_##TYPE##_typedef_t*);

#define def_fmi3_xml_get_type_display_unit(TYPE, TYPE_SIZE) \
    fmi3_xml_display_unit_t* fmi3_xml_get_##TYPE_SIZE##_type_display_unit(fmi3_xml_##TYPE##_typedef_t*);

#define def_fmi3_xml_get_type_XX_all(               TYPE, SIZE)         \
    def_fmi3_xml_get_type_XX(_min,                  TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_XX(_max,                  TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_XX(_nominal,              TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_unit(                     TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_is_relative_quantity(     TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_is_unbounded(             TYPE, TYPE##SIZE)   \
    def_fmi3_xml_get_type_display_unit(             TYPE, TYPE##SIZE)

def_fmi3_xml_get_type_XX_all(float, 64)
def_fmi3_xml_get_type_XX_all(float, 32)
