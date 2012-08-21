This files lists non-standard CMake modules used by the build system.

UseDoxygen.cmake
-------------------
Available from:
http://tobias.rautenkranz.ch/cmake/doxygen/

Modification: added configuration variable for images:

usedoxygen_set_default(DOXYFILE_IMAGE_DIR ${DOXYFILE_SOURCE_DIR}
		STRING "Doxygen images input directory")