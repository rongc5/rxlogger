#!/usr/bin/env bash
set -euo pipefail
root_dir=$(cd "$(dirname "$0")/.." && pwd)
cd "$root_dir"

mkdir -p build/bin build/lib logs

# Move build artifacts if present
if [[ -f liblogger.a ]]; then
  mv -f liblogger.a build/lib/
fi
if [[ -x test_logger ]]; then
  mv -f test_logger build/bin/
fi
if [[ -x style_test ]]; then
  mv -f style_test build/bin/
fi

# Move root-level .log files into logs/
shopt -s nullglob
for f in *.log; do
  if [[ "$f" != "logs" ]]; then
    mv -f "$f" logs/
  fi
done

echo "Organized artifacts -> build/{bin,lib}, logs/*.log"
