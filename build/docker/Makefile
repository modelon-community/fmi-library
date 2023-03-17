include $(CONFIG_FILE)

# Use this file to build FMI Library on docker.
# Overview of what happens:
# 1. An image that contains the current project is created and started
# 2. The build/docker/build.sh script is called from the container
# 3. This in turn calls the Makefile in project root
# 4. The build artifacts are copied to the mounted project root (in host)
#
# NOTE: Working directory when calling this file must be project root.

IMG_TAG=fmil:test

.PHONY: docker_image install test documentation docker_run_debug

docker_image:
	docker build -f build/docker/$(DOCKERFILE_NAME) -t $(IMG_TAG) --no-cache . # --no-cache ensures that we get the latest FMIL files

install test documentation: docker_image
	docker run \
		-w $(MOUNTED_DIR) \
		-u baseuser \
		-v /$(CURDIR):$(MOUNTED_DIR) \
		$(IMG_TAG) \
		//bin/bash -c "$(MOUNTED_DIR)/build/docker/build.sh \
			$@ $(BUILD_DIR) $(INSTALL_DIR) $(CONFIG)"