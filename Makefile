# This file wraps the CMake build system for FMI Library, and is used by
# Jenkins to build some predetermined configurations, do testing as a separate
# stage, and build the documentation.

# NOTE: 'nproc' doesn't exist on Windows, so just hardcoding "some" value for now.
NPROC=8

FMIL_REPO_DIR:=$(shell pwd)

include $(CONFIG_FILE)

.PHONY: install test documentation generate clean

install: generate
	cd $(BUILD_DIR) && \
		cmake --build . --parallel $(NPROC) --config $(BUILD_TYPE) --target install

documentation: generate
	cd $(BUILD_DIR) && \
		make doc

generate:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && \
		cmake -DCMAKE_INSTALL_PREFIX=../$(INSTALL_DIR) \
			-DFMILIB_GENERATE_DOXYGEN_DOC=$(GENERATE_DOXYGEN_DOC) \
			-DFMILIB_BUILD_WITH_STATIC_RTLIB=$(BUILD_WITH_STATIC_RTLIB) \
			-DFMILIB_BUILD_TESTS=$(BUILD_TESTS) \
			-DFMILIB_TEST_LOCALE=$(TEST_LOCALE) \
			-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
			$(FMILIB_CMAKE_CUSTOM_FLAGS) \
			-G $(GENERATOR) \
			../$(SRC_DIR)
	
test: test_unit test_installation

test_unit: generate
	cd $(BUILD_DIR) && \
		$(TEST_COMMAND)

test_installation: install
	set -eu; \
	cd Test/test_installation; \
	rm -rf build && mkdir -p build; \
	cd build; \
	cmake -DFMIL_HOME=$(FMIL_REPO_DIR)/$(INSTALL_DIR) -G $(GENERATOR) ..; \
	cmake --build .; \
	ctest $(CTEST_FLAGS_COMMON)

clean:
	rm -rf -v build_* install_*
