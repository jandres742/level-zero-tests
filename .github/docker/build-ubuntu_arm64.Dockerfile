# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: MIT

ARG IMAGE_VERSION=20_04_arm
FROM amr-registry.caas.intel.com/level-zero-linux-compute/ubuntu:$IMAGE_VERSION as base

RUN dpkg --add-architecture arm64

RUN echo "http_proxy=http://proxy-chain.intel.com:911" >> /etc/environment
RUN echo "https_proxy=http://proxy-chain.intel.com:911" >> /etc/environment
RUN echo "ftp_proxy=http://proxy-chain.intel.com:911" >> /etc/environment
RUN echo "socks_proxy=http://proxy-us.intel.com:1080" >> /etc/environment
RUN echo "no_proxy=intel.com,.intel.com,localhost,127.0.0.1" >> /etc/environment

RUN apt-get update && apt-get install -y \
        build-essential \
        ccache \
        clang-format-7 \
        clang-tidy \
        cmake \
        curl \
        git \
        libboost-log-dev:arm64 \
        libboost-program-options-dev:arm64 \
        libboost-timer-dev:arm64 \
        libboost-chrono-dev:arm64 \
        libboost-system-dev:arm64 \
        libpapi-dev:arm64 \
        libpng-dev:arm64 \
        libva-dev:arm64 \
        ninja-build \
        ocl-icd-opencl-dev \
        opencl-headers \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT [ "" ]
