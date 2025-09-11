#!/usr/bin/env bash
set -euo pipefail
root_dir=$(cd "$(dirname "$0")/.." && pwd)
cd "$root_dir/test"
rm -f logs/comprehensive_t.* custom_test*.log || true
../build/bin/comprehensive_test
