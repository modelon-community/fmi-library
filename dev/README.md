# Building
See instructions in file README.md.

# Dev Container
Two dockerfiles can be located in the directory `build/docker/`. Invoking `docker build . -f build/docker/Dockerfile_x` where `x` is either `centos` or `ubuntu` builds a container with all the required dependencies to build and test FMI Library. This also supports integration with various VS-code extensions. Note that for `centos` some of the tests depending on `locale` are disabled.
The default behavior for the VS-code container is configured in the file `.devcontainer/devcontainer.json`.