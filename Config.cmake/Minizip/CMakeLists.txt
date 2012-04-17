cmake_minimum_required(VERSION 2.8)

project(minizip C)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${THIRDPARTYLIBS}/Zlib/zlib-1.2.6" "${FMILibrary_BINARY_DIR}/zlib")

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

include_directories("${THIRDPARTYLIBS}/Zlib/zlib-1.2.6" "${FMILibrary_BINARY_DIR}/zlib")
set(SOURCE
  ${THIRDPARTYLIBS}/Minizip/minizip/ioapi.c
  ${THIRDPARTYLIBS}/Minizip/minizip/miniunz.c
  ${THIRDPARTYLIBS}/Minizip/minizip/minizip.c
  #${THIRDPARTYLIBS}/Minizip/minizip/mztools.c
  ${THIRDPARTYLIBS}/Minizip/minizip/unzip.c
  ${THIRDPARTYLIBS}/Minizip/minizip/zip.c
)

if(WIN32)
    set(SOURCE ${SOURCE} ${THIRDPARTYLIBS}/Minizip/minizip/iowin32.c)
endif(WIN32)

set(HEADERS
  ${THIRDPARTYLIBS}/Minizip/minizip/crypt.h
  ${THIRDPARTYLIBS}/Minizip/minizip/ioapi.h  
  ${THIRDPARTYLIBS}/Minizip/minizip/miniunz.h
  #${THIRDPARTYLIBS}/Minizip/minizip/mztools.h
  ${THIRDPARTYLIBS}/Minizip/minizip/unzip.h
)

if(WIN32)
    set(HEADERS ${HEADERS} ${THIRDPARTYLIBS}/Minizip/minizip/iowin32.h)
endif(WIN32)

add_library(minizip ${SOURCE} ${HEADERS})

target_link_libraries(minizip zlib)
