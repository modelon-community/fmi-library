cmake_minimum_required(VERSION 2.6)

project(UseDoxygen)

set(CMAKE_INSTALL_PREFIX ${CMAKE_ROOT})

install(FILES UseDoxygen.cmake Doxyfile.in
	DESTINATION "Modules")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
