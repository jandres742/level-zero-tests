# Copyright (C) 2021 Intel Corporation
# SPDX-License-Identifier: MIT

ARG IMAGE_VERSION=15.2
FROM amr-registry.caas.intel.com/level-zero-linux-compute/sles:$IMAGE_VERSION as base

USER root

# Static libraries for boost are not part of the SLES distribution
RUN git clone --recurse-submodules --branch boost-1.70.0 https://github.com/boostorg/boost.git && \
    cd boost && \
    ./bootstrap.sh && \
    ./b2 install \
      -j 4 \
      address-model=64 \
      --with-chrono \
      --with-log \
      --with-program_options \
      --with-serialization \
      --with-system \
      --with-timer && \
    cd .. && \
    rm -rf boost

ENTRYPOINT [ "" ]
