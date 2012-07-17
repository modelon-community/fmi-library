This files lists non-standard CMake modules used by the build system.

1. libutils.cmake
-----------------
libutils.cmake and merge_archives_unix.cmake.in were downloaded from:
http://www.cmake.org/pipermail/cmake/2010-May/036731.html

The libutils.cmake file was modified:
1. Adapted to use standard CMakeParseArguments module instead of MySQL one
2. Fixed to enable merging of libraries with different names in different 
configurations under MSVC (specifically zlib was problematic).

2. UseDoxygen.cmake
-------------------
Available from:
http://tobias.rautenkranz.ch/cmake/doxygen/

Modification: added configuration variable for images:

usedoxygen_set_default(DOXYFILE_IMAGE_DIR ${DOXYFILE_SOURCE_DIR}
		STRING "Doxygen images input directory")