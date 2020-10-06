# This file wraps the CMake build system for FMI Library, and is used by
# Jenkins to build some predetermined configurations, do testing as a separate
# stage, and build the documentation.

include $(CONFIG_FILE)

.PHONY: install test documentation generate clean

install: generate
	cd $(BUILD_DIR) && \
		cmake --build . --config MinSizeRel --target '$@' 

test: generate
	cd $(BUILD_DIR) && \
		ctest -C MinSizeRel

documentation: generate
	cd $(BUILD_DIR) && \
		make doc

generate:
	mkdir $(BUILD_DIR) || true
	cd $(BUILD_DIR) && \
		cmake -DFMILIB_INSTALL_PREFIX=../$(INSTALL_DIR) \
			-DFMILIB_GENERATE_DOXYGEN_DOC=$(GENERATE_DOXYGEN_DOC) \
			-DFMILIB_BUILD_WITH_STATIC_RTLIB=$(BUILD_WITH_STATIC_RTLIB) \
			-DFMILIB_BUILD_TESTS=$(BUILD_TESTS) \
			-DFMILIB_TEST_LOCALE=$(TEST_LOCALE) \
			$(FMILIB_CMAKE_CUSTOM_FLAGS) \
			-G $(GENERATOR) \
			../$(SRC_DIR)

clean:
	rm -rf -v build-* install-*
