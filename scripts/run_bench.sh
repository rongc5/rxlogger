#!/usr/bin/env bash
set -euo pipefail
root_dir=$(cd "$(dirname "$0")/.." && pwd)
cd "$root_dir"
./scripts/build_cmake.sh >/dev/null
cd test
../build/bin/perf_bench "${1:-10000}" "${2:-1}"

