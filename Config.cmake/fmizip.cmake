if(NOT FMIZIPDIR)
    set(FMIZIPDIR ${FMILIBRARYHOME}/src/ZIP)
	
    include(jmutil)
	
	set(DOXYFILE_EXTRA_SOURCES "${DOXYFILE_EXTRA_SOURCES} \"${FMIZIPDIR}/include\"")

    set(FMIZIP_LIBRARIES fmizip)
	
    add_subdirectory(Config.cmake/Minizip)
	
	include_directories("${FMIZIPDIR}/include" "${THIRDPARTYLIBS}/Minizip/minizip" "${THIRDPARTYLIBS}/FMI" "${THIRDPARTYLIBS}/Zlib/zlib-1.2.6" "${FMILibrary_BINARY_DIR}/zlib")

set(FMIZIPSOURCE
  ${FMIZIPDIR}/src/fmi_zip_unzip.c
  ${FMIZIPDIR}/src/fmi_zip_zip.c
)

set(FMIZIPHEADERS
#  src/fmi_zip_unzip_impl.h
  ${FMIZIPDIR}/include/fmi_zip_unzip.h
  ${FMIZIPDIR}/include/fmi_zip_zip.h
)

#include_directories("${THIRDPARTYLIBS}/zlib/lib/VS2005/win32")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DZLIB_STATIC")

add_library(fmizip ${FMILIBKIND} ${FMIZIPSOURCE} ${FMIZIPHEADERS})

target_link_libraries(fmizip minizip)

endif(NOT FMIZIPDIR)
