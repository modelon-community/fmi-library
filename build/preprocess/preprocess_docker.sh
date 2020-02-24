#!/bin/bash

# --- Script Description ---
#
# This script builds the docker image, launches a new container, and tells it to
# build all the files necessary for preprocessing.
#

set -eu -o pipefail -x

tag=cmake_preprocess

# build the image
docker build -t $tag .

# launch container that mounts project folder and then perform preprocessing
docker run -v "/$(pwd):/mount_host" \
		$tag \
        //bin/bash -c "cd /mount_host && chmod a+x preprocess.sh && ./preprocess.sh src/templates/x.c build_cmake/src/templates/x.c"

