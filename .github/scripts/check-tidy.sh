#!/usr/bin/env bash

set -euo pipefail

exec python3 ./scripts/lint/cpp.py --warnings-as-errors='*'
