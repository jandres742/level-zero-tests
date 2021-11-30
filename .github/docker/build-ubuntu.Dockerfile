# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: MIT

ARG IMAGE_VERSION=focal_20_04
FROM amr-registry.caas.intel.com/level-zero-linux-compute/ubuntu:$IMAGE_VERSION as base

FROM base AS branch-version-eoan-20200114
RUN sed -i 's/us.archive.ubuntu.com/old-releases.ubuntu.com/g' /etc/apt/sources.list
RUN sed -i 's/archive.ubuntu.com/old-releases.ubuntu.com/g' /etc/apt/sources.list
RUN sed -i 's/security.ubuntu.com/old-releases.ubuntu.com/g' /etc/apt/sources.list
ENV VAR=TRUE

FROM base AS branch-version-bionic-20200807
RUN echo "this is the stage that sets VAR=FALSE"
ENV VAR=FALSE

FROM base AS branch-version-focal_20_04
RUN echo "this is the stage that sets VAR=FALSE"
ENV VAR=FALSE

FROM branch-version-${IMAGE_VERSION} AS final
RUN echo "Eoan is requested is equal to ${VAR}"

RUN apt-get update && apt-get install -y \
        build-essential \
        ccache \
        clang-format-7 \
        clang-tidy \
        cmake \
        curl \
        git \
        libboost-all-dev \
        libpapi-dev \
        libpng-dev \
        libva-dev \
        ninja-build \
        ocl-icd-opencl-dev \
        opencl-headers \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT [ "" ]
