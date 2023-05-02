include $(CONFIG_FILE)

# This file builds FMI Library on docker.
#
# NOTE: Working directory when calling this file must be project root.

IMG_TAG=fmil:test

.PHONY: docker_image install test documentation docker_run_debug

docker_image:
	docker build -f build/docker/$(DOCKERFILE_NAME) -t $(IMG_TAG) .

install test documentation: docker_image
	docker run \
		-w $(MOUNTED_DIR) \
		-u baseuser \
		-v /$(CURDIR):$(MOUNTED_DIR) \
		$(IMG_TAG) \
		//bin/bash -c "make $@ CONFIG_FILE=./build/config/$(CONFIG)"