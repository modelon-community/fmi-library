#    Copyright (C) 2012 Modelon AB
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.
#
#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT FMIIMPORTDIR)
set(FMIIMPORTDIR ${FMILIBRARYHOME}/src/Import)
include(jmutil)
include(fmixml)
include(fmicapi)


################################################################################
set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIIMPORTDIR}/include\"")

set(FMIIMPORT_PUBHEADERS
    include/FMI1/fmi1_import.h
    include/FMI1/fmi1_import_capi.h
    include/FMI1/fmi1_import_capabilities.h
    include/FMI1/fmi1_import_cosim.h
    include/FMI1/fmi1_import_type.h
    include/FMI1/fmi1_import_unit.h
    include/FMI1/fmi1_import_variable.h
    include/FMI1/fmi1_import_variable_list.h
    include/FMI1/fmi1_import_vendor_annotations.h
    include/FMI1/fmi1_import_convenience.h

    include/FMI2/fmi2_import.h
    include/FMI2/fmi2_import_capi.h
    include/FMI2/fmi2_import_type.h
    include/FMI2/fmi2_import_unit.h
    include/FMI2/fmi2_import_variable.h
    include/FMI2/fmi2_import_variable_list.h
    include/FMI2/fmi2_import_convenience.h

    include/FMI3/fmi3_import.h
    include/FMI3/fmi3_import_capi.h
    include/FMI3/fmi3_import_type.h
    include/FMI3/fmi3_import_unit.h
    include/FMI3/fmi3_import_variable.h
    include/FMI3/fmi3_import_variable_list.h
    include/FMI3/fmi3_import_convenience.h

    include/FMI/fmi_import_context.h
    include/FMI/fmi_import_options.h
    include/FMI/fmi_import_util.h
)

set(FMIIMPORT_PRIVHEADERS
    src/FMI1/fmi1_import_impl.h
    src/FMI1/fmi1_import_variable_list_impl.h

    src/FMI2/fmi2_import_impl.h
    src/FMI2/fmi2_import_variable_list_impl.h

    src/FMI3/fmi3_import_impl.h
    src/FMI3/fmi3_import_priv.h
    src/FMI3/fmi3_import_variable_list_impl.h
)

PREFIXLIST(FMIIMPORT_PRIVHEADERS ${FMIIMPORTDIR}/)
PREFIXLIST(FMIIMPORT_PUBHEADERS  ${FMIIMPORTDIR}/)

set(FMIIMPORTHEADERS
    ${FMIIMPORT_PUBHEADERS} ${FMIIMPORT_PRIVHEADERS}
)

set(FMIIMPORTSOURCE
    src/FMI/fmi_import_context.c
    src/FMI/fmi_import_options.c
    src/FMI/fmi_import_util.c

    src/FMI1/fmi1_import_cosim.c
    src/FMI1/fmi1_import_capi.c
    src/FMI1/fmi1_import_type.c
    src/FMI1/fmi1_import_unit.c
    src/FMI1/fmi1_import_variable.c
    src/FMI1/fmi1_import_variable_list.c
    src/FMI1/fmi1_import_vendor_annotations.c
    src/FMI1/fmi1_import.c
    src/FMI1/fmi1_import_capabilities.c
    src/FMI1/fmi1_import_convenience.c

    src/FMI2/fmi2_import_capi.c
    src/FMI2/fmi2_import_type.c
    src/FMI2/fmi2_import_unit.c
    src/FMI2/fmi2_import_variable.c
    src/FMI2/fmi2_import_variable_list.c
    src/FMI2/fmi2_import.c
    src/FMI2/fmi2_import_convenience.c

    src/FMI3/fmi3_import_capi.c
    src/FMI3/fmi3_import_type.c
    src/FMI3/fmi3_import_unit.c
    src/FMI3/fmi3_import_variable.c
    src/FMI3/fmi3_import_variable_list.c
    src/FMI3/fmi3_import.c
    src/FMI3/fmi3_import_priv.c
    src/FMI3/fmi3_import_convenience.c
)

PREFIXLIST(FMIIMPORTSOURCE  ${FMIIMPORTDIR}/)

set(FMIIMPORT_PUBLIC_INCLUDE_DIR ${FMIIMPORTDIR}/include)

# XXX:
# Since target fmilib_shared essentially is fmiimport + jmutils, we need
# to make all the properties of these sub-libraries available when building
# that target.
set(FMIIMPORT_PRIVATE_INCLUDE_DIRS
    ${FMIIMPORTDIR}/src
    ${FMILIB_THIRDPARTYLIBS}/FMI
    ${EXPAT_INCLUDE_DIRS}  # FIXME: XML_Parser is used in fmi_import_context_impl.h
)
set(FMIIMPORT_PUBLIC_INCLUDE_DIRS
    ${FMILIB_CONFIG_INCLUDE_DIR}
    ${FMIIMPORT_PUBLIC_INCLUDE_DIR}
)

add_library(fmiimport STATIC ${FMIIMPORTSOURCE} ${FMIIMPORTHEADERS})
target_link_libraries(fmiimport
    PRIVATE jmutils fmixml fmizip fmicapi
)
target_include_directories(fmiimport
    PRIVATE ${FMIIMPORT_PRIVATE_INCLUDE_DIRS}
    PUBLIC  ${FMIIMPORT_PUBLIC_INCLUDE_DIR}
)

endif(NOT FMIIMPORTDIR)
