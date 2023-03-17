FMI Library
===========

FMI Library (FMIL) is a software package written in C that enables integration
of Functional Mock-up Units (FMUs) import in applications. FMI Library is an
independent open-source implementation of the [FMI open standard](https://fmi-standard.org).

The documentation for the latest version of FMI Library can be downloaded from the [release page](https://github.com/modelon-community/fmi-library/releases).

Please report issues related to FMI Library to the [GitHub issues page](https://github.com/modelon-community/fmi-library/issues).

For information about contributing, see [here](https://github.com/modelon/contributing).

# Building
Instructions on how to build FMI Library can be found in the HTML documentation for each release. [See the release page.](https://github.com/modelon-community/fmi-library/releases)

# Dev Container
Two dockerfiles can be located in the directory `build/docker/`. Invoking `docker build . -f build/docker/Dockerfile_x` where `x` is either `centos` or `ubuntu` builds a container with all the required dependencies to build and test FMI Library. This also supports integration with various VS-code extensions. Note that for `centos` some of the tests depending on `locale` are disabled.