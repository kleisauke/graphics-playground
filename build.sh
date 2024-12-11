#!/usr/bin/env bash
set -e

SOURCE_DIR=$PWD

# Build within the mounted volume, handy for debugging
# and ensures that dependencies are not being rebuilt
BUILD_DIR=$SOURCE_DIR/build

DEPS=$BUILD_DIR/deps
rm -rf $DEPS/
mkdir -p $DEPS

# Define default arguments
# This can be overridden with:
# --build-type MinSizeRel
# --build-type Debug
BUILD_TYPE=Release

# Parse arguments
while [ $# -gt 0 ]; do
  case $1 in
    --build-type) BUILD_TYPE="$2"; shift ;;
    *) echo "ERROR: Unknown parameter: $1" >&2; exit 1 ;;
  esac
  shift
done

# Common compiler flags
export CFLAGS="-fno-rtti -fno-exceptions -mnontrapping-fptoint -msimd128 -DCORRADE_NO_ASSERT"
if [ "$BUILD_TYPE" = "Debug" ]; then export CFLAGS+=" -gsource-map"; fi
export CXXFLAGS="$CFLAGS"

echo "============================================="
echo "Environment"
echo "============================================="
emcc --version
node --version

echo "============================================="
echo "Compiling playground"
echo "============================================="
(
  mkdir -p $DEPS/playground
  cd $DEPS/playground
  emcmake cmake $SOURCE_DIR -Wno-dev -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$SOURCE_DIR 
  cmake --build . -- -j$(nproc)
  cmake --install . --component playground
)
