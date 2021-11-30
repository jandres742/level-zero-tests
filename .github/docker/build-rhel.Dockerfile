# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: MIT

ARG IMAGE_VERSION=8.4
FROM amr-registry.caas.intel.com/level-zero-linux-compute/rhel:$IMAGE_VERSION as base

USER root

RUN yum update

RUN yum group install "Development Tools" -y

RUN yum install -y \
        clang \
        cmake \
        curl \
        git \
        boost \
        boost-devel \
        boost-static \
        papi-devel \
        libpng-devel \
        libva-devel \
        ninja-build \
        opencl-headers

ENTRYPOINT [ "" ]
