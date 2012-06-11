#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 3 of the License.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
 FMI1/fmi1_callbacks.c
 FMI1/fmi1_enums.c
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
  FMI1/fmi1_functions.h
  FMI1/fmi1_types.h
  FMI1/fmi1_callbacks.h
  FMI1/fmi1_enums.h
)

PREFIXLIST(JMUTILSOURCE  ${JMUTILDIR}/src/)
PREFIXLIST(JMUTILHEADERS ${JMUTILDIR}/include/)

add_library(jmutils ${FMILIBKIND} ${JMUTILSOURCE} ${JMUTILHEADERS})

if(CYGWIN) 
message("not tested")
endif(CYGWIN)

if(UNIX) 
	message(STATUS "Adding -ldl to the linker")
	target_link_libraries(jmutils dl)
	#target_link_libraries(dl)
endif(UNIX)

endif(NOT JMUTILDIR)
