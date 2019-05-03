#!/bin/bash

# Build base image
file_base="$(dirname $0)/Dockerfile_ubuntu16_base"
tag_base='fmil_ubuntu16:latest'
docker build -f $file_base -t $tag_base .

# Build FMIL image from base
file_linux64="$(dirname $0)/Dockerfile_fmil_linux64"
tag_linux64='fmil_linux64'
docker build -f $file_linux64 -t $tag_linux64 .

# Build FMIL image from base
file_doc="$(dirname $0)/Dockerfile_fmil_documentation"
tag_doc='fmil_documentation'
docker build -f $file_doc -t $tag_doc .

