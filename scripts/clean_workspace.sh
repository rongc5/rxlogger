#!/usr/bin/env bash
set -euo pipefail

echo "[RxLogger] Cleaning workspace clutter..."
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
cd "$ROOT_DIR"

# Root-level artifacts
rm -rf logs 2>/dev/null || true
rm -f *.log 2>/dev/null || true
rm -f test_* 2>/dev/null || true

# Legacy build dirs
rm -rf obj liblogger.a 2>/dev/null || true

echo "[RxLogger] Done."

