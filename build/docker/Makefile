include $(CONFIG_FILE)

# NOTE: Working directory when calling this file must be project root.

# arg1: target name for the <project-root>/Makefile
define docker_run
	docker run \
		-v "$(shell pwd):/$(ARTIFACT_DIR)" \
		$(DOCKER_IMAGE_FMIL) \
		//bin/bash -c "build/docker/build.sh \
			$(1) $(BUILD_DIR) $(INSTALL_DIR) $(ARTIFACT_DIR) $(CONFIG)"
endef

# Change function call to docker_run to this function instead for debugging
define docker_run_debug
	winpty docker run -it \
		-v "/$(shell pwd):/$(ARTIFACT_DIR)" \
		$(DOCKER_IMAGE_FMIL)
endef

docker_image:
	docker build -f build/docker/Dockerfile_base -t $(DOCKER_IMAGE_BASE) .
	docker build -f build/docker/Dockerfile_fmil -t $(DOCKER_IMAGE_FMIL) --no-cache . # --no-cache ensures that we get the latest FMIL files

install test documentation: docker_image
	$(call docker_run, $@)

