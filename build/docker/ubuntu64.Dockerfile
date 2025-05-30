FROM ubuntu:22.04

# DEBIAN_FRONTEND is used to deal with dependency 'tzdata' that is installed
# when 'cmake' is installed.
ENV DEBIAN_FRONTEND=noninteractive

USER root

# Install development tools:
# (curl is used by some VSCode extensions)
RUN apt-get update && apt-get install -yq    \
    vim          tmux      doxygen           \
    cmake        g++       language-pack-sv  \
    git          curl      valgrind          \
    ninja-build  sudo

# Give baseuser appropriate permissions
RUN adduser baseuser; \
    passwd  -d "baseuser"; \
    passwd  -d "root"; \
    usermod -a -G sudo baseuser

# Switch to baseuser
USER baseuser

# Personalization:
RUN echo '[[ -f /mnt/dev/bashrc_personal ]] && . /mnt/dev/bashrc_personal' >> ~/.bashrc; \
    [[ ! -f ~/.inputrc ]] && echo '$include /etc/inputrc'              > ~/.inputrc; \
                             echo '$include /mnt/dev/inputrc_personal' > ~/.inputrc