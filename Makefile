include $(CONFIG_FILE)

install_docker test_docker documentation_docker:
	make -C . -f build/docker/Makefile $(subst _docker,,$@) CONFIG_FILE="build/config/$(CONFIG)"

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
			-G $(GENERATOR) \
			../$(SRC_DIR)

clean:
	rm -rf -v build-* install-*
