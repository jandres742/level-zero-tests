#! /usr/bin/env bash

set -eu

path=$1
name=$2
version=$3
src=$4

if dt asset execute -- size \
  --root-url https://gfx-assets.fm.intel.com/artifactory \
  $path $name $version >/dev/null 2>&1; then
    echo "asset already exists, skipping publish"
else
    dt asset publish \
      --root-url https://gfx-assets.fm.intel.com/artifactory \
      $path $name $version $src
fi
