FROM mcr.microsoft.com/devcontainers/cpp:0-ubuntu-22.04

# Install additional packages
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    neovim \
    python3-pip \
    libomp-dev \
    libx11-dev \
    libglfw3-dev \
    cmake \
    valgrind \
    imagemagick \
    && pip3 install pynvim neovim-remote

# Clean up
RUN apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*
