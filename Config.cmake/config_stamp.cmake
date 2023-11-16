FIND_PACKAGE(Subversion REQUIRED)
Subversion_WC_INFO(${CMAKE_SOURCE_DIR} FMILibrary)

message(STATUS "Generating build timestamp in ${CMAKE_BINARY_DIR}/config_fmilib.c")
configure_file (
    "${CMAKE_SOURCE_DIR}/Config.cmake/config_fmilib.c.cmake"
    "${CMAKE_BINARY_DIR}/config_fmilib.c"
)
