if(NOT FMICAPIDIR)
set(FMICAPIDIR ${FMILIBRARYHOME}/src/CAPI)

set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMICAPIDIR}/include\"")

include(jmutil)

include_directories("${FMICAPIDIR}/include" "${FMICAPIDIR}/src")
set(FMICAPI_LIBRARIES fmicapi)

set(FMICAPISOURCE
    src/FMI1/fmi1_capi_cs.c
    src/FMI1/fmi1_capi_me.c
    src/FMI1/fmi1_capi.c
)
set(FMICAPIHEADERS
	include/FMI1/fmi1_capi.h	
	src/FMI1/fmi1_capi_impl.h
)
 
include_directories("${FMILIB_THIRDPARTYLIBS}/FMI/")

PREFIXLIST(FMICAPISOURCE  ${FMICAPIDIR}/)
PREFIXLIST(FMICAPIHEADERS ${FMICAPIDIR}/)

add_library(fmicapi ${FMILIBKIND} ${FMICAPISOURCE} ${FMICAPIHEADERS})

target_link_libraries(fmicapi ${JMUTIL_LIBRARIES})

# install(DIRECTORY ${FMIXMLDIR}/include DESTINATION .)
# install(DIRECTORY ${FMICAPIDIR}/include DESTINATION .)
#install(DIRECTORY ${JMRUNTIMEHOME}/FMI/ZIP/include DESTINATION include)

endif(NOT FMICAPIDIR)
