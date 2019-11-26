include $(CONFIG_FILE)

# Use this file to build FMI Library on docker.
# Overview of what happens:
# 1. An image that contains the current project is created and started
# 2. The build/docker/build.sh script is called from the container
# 3. This in turn calls the Makefile in project root
# 4. The build artifacts are copied to the mounted project root (in host)
# 
# NOTE: Working directory when calling this file must be project root.

.PHONY: docker_image install test documentation docker_run_debug

docker_image:
	docker build -f build/docker/Dockerfile_base -t $(DOCKER_IMAGE_BASE) .
	docker build -f build/docker/Dockerfile_fmil -t $(DOCKER_IMAGE_FMIL) --no-cache . # --no-cache ensures that we get the latest FMIL files

install test documentation: docker_image
	docker run \
		-v "/$(CURDIR):$(ARTIFACT_DIR)" \
		$(DOCKER_IMAGE_FMIL) \
		//bin/bash -c "build/docker/build.sh \
			$@ $(BUILD_DIR) $(INSTALL_DIR) $(ARTIFACT_DIR) $(CONFIG)"

docker_run_debug: docker_image
	winpty docker run -it \
		-v "/$(shell pwd):$(ARTIFACT_DIR)" \
		$(DOCKER_IMAGE_FMIL)

