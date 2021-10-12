#!/usr/bin/env bash
set -e

SOURCE_DIR=$PWD

# Build within the mounted volume, handy for debugging
# and ensures that dependencies are not being rebuilt
BUILD_DIR=$SOURCE_DIR/build

DEPS=$BUILD_DIR/deps
TARGET=$BUILD_DIR/target
rm -rf $DEPS/
mkdir -p $DEPS
mkdir -p $TARGET

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

# Dependency version numbers
# Note: keep in-sync with third_party/corrade
VERSION_CORRADE=bb626d6

echo "============================================="
echo "Environment"
echo "============================================="
emcc --version

echo "============================================="
echo "Compiling native corrade-rc"
echo "============================================="
test -f "$TARGET/bin/corrade-rc" || (
  mkdir -p $DEPS/corrade-rc
  curl -Ls https://github.com/mosra/corrade/archive/$VERSION_CORRADE.tar.gz | tar xzC $DEPS/corrade-rc --strip-components=1
  cd $DEPS/corrade-rc
  cmake -B_build -H. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$TARGET \
    -DWITH_INTERCONNECT=OFF -DWITH_PLUGINMANAGER=OFF -DWITH_TESTSUITE=OFF -DWITH_UTILITY=OFF
  make -C _build install
)

echo "============================================="
echo "Compiling playground"
echo "============================================="
(
  mkdir -p $DEPS/playground
  cd $DEPS/playground
  emcmake cmake $SOURCE_DIR -Wno-dev -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$SOURCE_DIR/dist" \
    -DCORRADE_RC_EXECUTABLE=$TARGET/bin/corrade-rc
  make
)
