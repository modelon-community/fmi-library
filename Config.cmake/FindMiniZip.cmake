# Find the Minizip library
# Defines:

#  MINIZIP_INCLUDE_DIR - minizip include directory
#  MINIZIP_LIBRARY     - minizip library file
#  MINIZIP_FOUND       - TRUE if minizip is found

find_path(MINIZIP_INCLUDE_DIR NAMES unzip.h PATH_SUFFIXES minizip)

if(MINIZIP_USE_STATIC_LIBS)
  find_library(MINIZIP_LIBRARY NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}minizip${CMAKE_STATIC_LIBRARY_SUFFIX} minizip)
else()
  find_library(MINIZIP_LIBRARY NAMES minizip)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MiniZip DEFAULT_MSG MINIZIP_LIBRARY MINIZIP_INCLUDE_DIR)
