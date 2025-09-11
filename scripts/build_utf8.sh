#!/usr/bin/env bash
set -euo pipefail
export CXXFLAGS_ADD="-finput-charset=GBK -fexec-charset=UTF-8"
make clean || true
make CXXFLAGS+=" ${CXXFLAGS_ADD} " -j"$(nproc)"

