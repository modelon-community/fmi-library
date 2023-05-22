# This file wraps the CMake build system for FMI Library, and is used by
# Jenkins to build some predetermined configurations, do testing as a separate
# stage, and build the documentation.

include $(CONFIG_FILE)

.PHONY: install test documentation generate clean

install: generate
	cd $(BUILD_DIR) && \
		cmake --build . --parallel $(shell nproc) --config $(BUILD_TYPE) --target '$@'

# Note that this does not execute the test binaries that are using the test framework Catch2
test: generate
	cd $(BUILD_DIR) && \
		$(TEST_COMMAND)

documentation: generate
	cd $(BUILD_DIR) && \
		make doc

generate:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
		cmake -DFMILIB_INSTALL_PREFIX=../$(INSTALL_DIR) \
			-DFMILIB_GENERATE_DOXYGEN_DOC=$(GENERATE_DOXYGEN_DOC) \
			-DFMILIB_BUILD_WITH_STATIC_RTLIB=$(BUILD_WITH_STATIC_RTLIB) \
			-DFMILIB_BUILD_TESTS=$(BUILD_TESTS) \
			-DFMILIB_TEST_LOCALE=$(TEST_LOCALE) \
			-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
			$(FMILIB_CMAKE_CUSTOM_FLAGS) \
			-G $(GENERATOR) \
			../$(SRC_DIR)

clean:
	rm -rf -v build_* install_*
