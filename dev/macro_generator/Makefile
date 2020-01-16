TAG := macro_generator

BUILD_DIR := build_make
EMPTY_TARGETS := $(BUILD_DIR)/empty_targets
IMAGE_ET := $(EMPTY_TARGETS)/image_et


$(info $(CURDIR))

.PHONY: image run all

all: run

run: image
	docker run \
		-v "/$(CURDIR)/mount_me:/mount_host" \
		$(TAG) \
		//bin/bash -c "cd /mount_host && chmod a+x run.sh && ./run.sh"

image: $(IMAGE_ET)

$(IMAGE_ET): Dockerfile Makefile
	docker build -t $(TAG) .
	mkdir -p $(EMPTY_TARGETS)
	touch $(IMAGE_ET)

