if(NOT JMUTILDIR)
    set(JMUTILDIR ${FMILIBRARYHOME}/src/Util/)
    set(JMUTIL_LIBRARIES jmutils)
    include_directories ("${JMUTILDIR}/include" "${FMILIB_THIRDPARTYLIBS}/FMI/")
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${JMUTILDIR}\"")

set(JMUTILSOURCE
 Common/jm_callbacks.c
 Common/jm_templates_inst.c
 Common/jm_named_ptr.c
 Common/jm_wc_match.c
 Common/jm_portability.c
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
  Common/jm_wc_match.h
  Common/jm_portability.h
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
