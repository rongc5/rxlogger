#!/usr/bin/env bash
set -euo pipefail
root_dir=$(cd "$(dirname "$0")/.." && pwd)
cd "$root_dir"
make -s -j"$(nproc)"
g++ -std=c++11 -Wall -O2 -pthread -Iinclude test/style_test.cpp liblogger.a -o style_test
./style_test
echo '--- style_test.db (tail) ---'
tail -n 5 logs/style_test.db || true
echo '--- style_custom.log (tail) ---'
tail -n 5 style_custom.log || true

