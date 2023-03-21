FROM centos:7.4.1708

ARG CMAKE_VERSION=3.25.2
ARG CMAKE_DIR=cmake-${CMAKE_VERSION}-linux-x86_64
ARG CMAKE_TAR=cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz

USER root

# Give baseuser appropriate permissions
RUN adduser baseuser && \
    passwd  -d baseuser && \
    passwd  -d root && \
    usermod -aG wheel baseuser

# Install development tools
# curl is used by certain VSCode extensions
RUN yum install -yq \
    vim     tmux    which   wget \
    doxygen make    gcc-c++ git  \
    curl    valgrind

# Install CMake 3 as an alternative with higher priority, by default CentOS 7 installs CMake 2.
RUN wget -q https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_TAR} && \
    tar -xzf ${CMAKE_TAR} -C /opt && \
    ln -s /opt/${CMAKE_DIR}/bin/cmake /usr/local/bin/cmake && \
    ln -s /opt/${CMAKE_DIR}/bin/ccmake /usr/local/bin/ccmake && \
    ln -s /opt/${CMAKE_DIR}/bin/ctest /usr/local/bin/ctest && \
    ln -s /opt/${CMAKE_DIR}/bin/cpack /usr/local/bin/cpack && \
    rm ${CMAKE_TAR}

# Switch to baseuser
USER baseuser

# Personalization:
RUN echo '[[ -f /mnt/dev/bashrc_personal ]] && . /mnt/dev/bashrc_personal' >> ~/.bashrc; \
    [[ ! -f ~/.inputrc ]] && echo '$include /etc/inputrc'              > ~/.inputrc; \
                             echo '$include /mnt/dev/inputrc_personal' > ~/.inputrc