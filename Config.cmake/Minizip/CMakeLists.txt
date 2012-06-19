cmake_minimum_required(VERSION 2.8)

project(minizip C)

set(BUILD_SHARED_LIBS OFF)
set(SKIP_INSTALL_HEADERS ON)
set(SKIP_INSTALL_FILES ON)
if(NOT FMILIB_INSTALL_SUBLIBS)
	set(SKIP_INSTALL_LIBRARIES ON)
endif()
add_subdirectory("${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.2.6" "${FMILibrary_BINARY_DIR}/zlib")

if(CMAKE_CL_64)
	set_target_properties(zlib PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
endif()

if(CMAKE_HOST_APPLE)
set(PLATFORM __APPLE__)
elseif(CMAKE_HOST_UNIX)
set(PLATFORM unix)
elseif(CMAKE_HOST_WIN32)
set(PLATFORM _WIN32)
else(CMAKE_HOST_APPLE)
message(FATAL_ERROR "Not supported Platform")
endif(CMAKE_HOST_APPLE)

add_definitions(-D${PLATFORM})

if(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

include_directories("${FMILIB_THIRDPARTYLIBS}/Zlib/zlib-1.2.6" "${FMILibrary_BINARY_DIR}/zlib")
set(SOURCE
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/ioapi.c
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/miniunz.c
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/minizip.c
  #${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/mztools.c
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/unzip.c
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/zip.c
)

if(WIN32)
    set(SOURCE ${SOURCE} ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/iowin32.c)
endif(WIN32)

set(HEADERS
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/crypt.h
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/ioapi.h  
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/miniunz.h
  #${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/mztools.h
  ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/unzip.h
)

if(WIN32)
    set(HEADERS ${HEADERS} ${FMILIB_THIRDPARTYLIBS}/Minizip/minizip/iowin32.h)
endif(WIN32)

add_library(minizip ${SOURCE} ${HEADERS})

target_link_libraries(minizip zlib)

if(FMILIB_INSTALL_SUBLIBS)
	install(TARGETS minizip
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib 
	)
endif()

