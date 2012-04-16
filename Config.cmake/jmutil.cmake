if(NOT JMUTILDIR)
    set(JMUTILDIR ${FMILIBRARYHOME}/src/Util/)
    set(JMUTIL_LIBRARIES jm_utils)
    include_directories (${FMILIBRARYHOME}/src/Util "${THIRDPARTYLIBS}/FMI/")
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${JMUTILDIR}\"")

set(JMUTILSOURCE
 jm_callbacks.c
 jm_templates_inst.c
 jm_named_ptr.c
 jm_wc_match.c
 jm_portability.c
 FMI1/fmi1_callbacks.c
 FMI1/fmi1_enums.c
)

set(JMUTILHEADERS
  jm_callbacks.h
  jm_vector.h
  jm_vector_template.h
  jm_stack.h
  jm_types.h
  jm_named_ptr.h
  jm_string_set.h
  jm_wc_match.h
  jm_portability.h
  FMI1/fmi1_functions.h
  FMI1/fmi1_types.h
  FMI1/fmi1_callbacks.h
  FMI1/fmi1_enums.h
)

PREFIXLIST(JMUTILSOURCE  ${JMUTILDIR})
PREFIXLIST(JMUTILHEADERS ${JMUTILDIR})

add_library(jm_utils ${JMUTILSOURCE} ${JMUTILHEADERS})

if(CYGWIN) 
message("not tested")
endif(CYGWIN)

if(UNIX) 
	link_libraries(dl)
	#target_link_libraries(dl)
endif(UNIX)

endif(NOT JMUTILDIR)
