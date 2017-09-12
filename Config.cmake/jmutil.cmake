#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT JMUTILDIR)
    set(JMUTILDIR ${FMILIBRARYHOME}/src/Util/)
    set(JMUTIL_LIBRARIES jmutils)
    include_directories ("${JMUTILDIR}/include" "${FMILIB_FMI_STANDARD_HEADERS}")
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${JMUTILDIR}/include\"")

set(JMUTILSOURCE
 JM/jm_callbacks.c
 JM/jm_templates_inst.c
 JM/jm_named_ptr.c
 JM/jm_portability.c
 FMI/fmi_version.c
 FMI/fmi_util.c
 
 FMI1/fmi1_enums.c
 FMI2/fmi2_enums.c
)

set(JMUTILHEADERS
  JM/jm_callbacks.h
  JM/jm_vector.h
  JM/jm_vector_template.h
  JM/jm_stack.h
  JM/jm_types.h
  JM/jm_named_ptr.h
  JM/jm_string_set.h
  JM/jm_portability.h
  FMI/fmi_version.h
  FMI/fmi_util.h

  FMI1/fmi1_functions.h
  FMI1/fmi1_types.h
  FMI1/fmi1_enums.h

  FMI2/fmi2_functions.h
  FMI2/fmi2_types.h
  FMI2/fmi2_enums.h
  FMI2/fmi2_xml_callbacks.h
)

PREFIXLIST(JMUTILSOURCE  ${JMUTILDIR}/src/)
PREFIXLIST(JMUTILHEADERS ${JMUTILDIR}/include/)

add_library(jmutils ${FMILIBKIND} ${JMUTILSOURCE} ${JMUTILHEADERS})

if(CYGWIN) 
message("not tested")
endif(CYGWIN)

target_link_libraries(jmutils c99snprintf)

if(UNIX)
    target_compile_definitions(jmutils PRIVATE -D_GNU_SOURCE)
	target_link_libraries(jmutils dl)
endif(UNIX)
if(WIN32)
	target_link_libraries(jmutils Shlwapi)
endif(WIN32)

endif(NOT JMUTILDIR)
