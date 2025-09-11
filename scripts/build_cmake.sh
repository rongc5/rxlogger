#!/bin/bash

# Modern CMake build helper for RxLogger
# Usage: ./scripts/build_cmake.sh [clean]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"

cd "$PROJECT_ROOT"

echo "[RxLogger] Project root: $PROJECT_ROOT"

if [[ "$1" == "clean" ]]; then
    echo "[RxLogger] Cleaning build directory $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    exit 0
fi

# Configure & build with CMake
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "[RxLogger] Running CMake configure ..."
cmake -DRXLOGGER_GBK_SOURCE=OFF ..

echo "[RxLogger] Building ..."
make -j"$(nproc)"

echo "[RxLogger] Build completed. Binaries are in $BUILD_DIR"
