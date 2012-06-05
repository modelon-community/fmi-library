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
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${JMUTILDIR}\"")

set(JMUTILSOURCE
 Common/jm_callbacks.c
 Common/jm_templates_inst.c
 Common/jm_named_ptr.c
 Common/jm_portability.c
 Common/fmi_version.c
 FMI1/fmi1_callbacks.c
 FMI1/fmi1_enums.c
)

set(JMUTILHEADERS
  Common/jm_callbacks.h
  Common/jm_vector.h
  Common/jm_vector_template.h
  Common/jm_stack.h
  Common/jm_types.h
  Common/jm_named_ptr.h
  Common/jm_string_set.h
  Common/jm_portability.h
  Common/fmi_version.h
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
