cmake_minimum_required(VERSION 3.15)

project(minizip C)

set(BUILD_SHARED_LIBS OFF)
set(SKIP_INSTALL_HEADERS ON)
set(SKIP_INSTALL_FILES ON)
if(NOT FMILIB_INSTALL_SUBLIBS)
    set(SKIP_INSTALL_LIBRARIES ON)
endif()

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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

set(SOURCE
    ${FMILIB_MINIZIP_SOURCE_DIR}/ioapi.c
    #${FMILIB_MINIZIP_SOURCE_DIR}/mztools.c
    ${FMILIB_MINIZIP_SOURCE_DIR}/unzip.c
    ${FMILIB_MINIZIP_SOURCE_DIR}/zip.c
)

if(WIN32)
    set(SOURCE ${SOURCE} ${FMILIB_MINIZIP_SOURCE_DIR}/iowin32.c)
endif(WIN32)

set(HEADERS
    ${FMILIB_MINIZIP_INCLUDE_DIR}/crypt.h
    ${FMILIB_MINIZIP_INCLUDE_DIR}/ioapi.h
    #${FMILIB_MINIZIP_INCLUDE_DIR}/mztools.h
    ${FMILIB_MINIZIP_INCLUDE_DIR}/unzip.h
)

if(WIN32)
    set(HEADERS ${HEADERS} ${FMILIB_MINIZIP_INCLUDE_DIR}/iowin32.h)
endif(WIN32)

add_library(minizip ${SOURCE} ${HEADERS})

target_link_libraries(minizip PUBLIC zlib)

target_include_directories(minizip
    PRIVATE ${ZLIB_INCLUDE_DIRS}
)

if(FMILIB_INSTALL_SUBLIBS)
    install(TARGETS minizip
          ARCHIVE DESTINATION lib
          LIBRARY DESTINATION lib
    )
endif()
