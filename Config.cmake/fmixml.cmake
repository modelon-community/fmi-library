if(NOT FMIXMLDIR)
set(FMIXMLDIR ${FMILIBRARYHOME}/src/XML/)
include(jmutil)

set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIXMLDIR}/include\"")

include_directories("${FMIXMLDIR}/include" "${THIRDPARTYLIBS}/FMI/")
set(FMIXML_LIBRARIES fmixml)

set(FMIXMLHEADERS
    include/FMI1/fmi1_xml_model_description.h
    src/FMI1/fmi1_xml_model_description_impl.h
    src/FMI1/fmi1_xml_parser.h
    include/FMI1/fmi1_xml_type.h
    src/FMI1/fmi1_xml_type_impl.h
    include/FMI1/fmi1_xml_unit.h
    src/FMI1/fmi1_xml_unit_impl.h
    include/FMI1/fmi1_xml_vendor_annotations.h
    src/FMI1/fmi1_xml_vendor_annotations_impl.h
    include/FMI1/fmi1_xml_variable.h
    src/FMI1/fmi1_xml_variable_impl.h
    # include/FMI1/fmi1_xml_variable_list.h
    # src/FMI1/fmi1_xml_variable_list_impl.h
    include/FMI1/fmi1_xml_capabilities.h
    src/FMI1/fmi1_xml_capabilities_impl.h
	# src/FMI1/fmi1_xml_query.h
	include/Common/fmi_xml_context.h
	src/Common/fmi_xml_context_impl.h
 )

set(FMIXMLSOURCE
    src/FMI1/fmi1_xml_parser.c
    src/FMI1/fmi1_xml_model_description.c
    src/FMI1/fmi1_xml_type.c
    src/FMI1/fmi1_xml_unit.c
    src/FMI1/fmi1_xml_vendor_annotations.c
    src/FMI1/fmi1_xml_variable.c
    # src/FMI1/fmi1_xml_variable_list.c
    src/FMI1/fmi1_xml_capabilities.c
    src/FMI1/fmi1_xml_cosim.c
	# src/FMI1/fmi1_xml_query.c
	src/Common/fmi_xml_context.c
)

#message(STATUS "Looking for installed expat library")
#find_package (EXPAT)

if(NOT EXPAT_FOUND)
  debug_message("Adding expat as sub-project")
  include_directories(${CMAKE_BINARY_DIR}/Expat)
  option(BUILD_tools OFF )
  option(BUILD_examples OFF )
  option(BUILD_tests OFF )
  option(BUILD_shared OFF )
  option(XML_DTD OFF)
  option(XML_NS OFF)
  mark_as_advanced(FORCE BUILD_tools BUILD_examples BUILD_tests BUILD_shared XML_DTD XML_NS)
  add_subdirectory(${THIRDPARTYLIBS}/Expat/expat-2.0.1 Expat)
 
	set(EXPAT_INCLUDE_DIRS "${THIRDPARTYLIBS}/Expat/expat-2.0.1/lib")
	set(EXPAT_FOUND true FORCE)
    set(EXPAT_LIBRARIES expat)
				
endif(NOT EXPAT_FOUND)

if(EXPAT_FOUND)
    include_directories(${EXPAT_INCLUDE_DIRS})
else(EXPAT_FOUND)
     message(FATAL_ERROR "No expat library for this platform")
endif(EXPAT_FOUND)

include_directories("${THIRDPARTYLIBS}/FMI/")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DXML_STATIC -DFMI_XML_QUERY")

PREFIXLIST(FMIXMLSOURCE  ${FMIXMLDIR}/)
PREFIXLIST(FMIXMLHEADERS ${FMIXMLDIR}/)

add_library(fmixml ${FMIXMLSOURCE} ${FMIXMLHEADERS})

target_link_libraries(fmixml ${JMUTIL_LIBRARIES} ${EXPAT_LIBRARIES})

endif(NOT FMIXMLDIR)
