#!/usr/bin/env bash
# Compare two labeled benchmark runs using Google Benchmark's own compare.py
# (proper Mann-Whitney U test across repetitions, not just a raw % delta).
# Needs tools/.venv - set up once with:
#   python3 -m venv tools/.venv
#   tools/.venv/bin/pip install -r src/benchmark/tools/requirements.txt
#
# Usage:
#   tools/compare.sh <before-label> <after-label>
set -euo pipefail
cd "$(dirname "$0")/.."

before="tools/bench-results/${1:?usage: tools/compare.sh <before-label> <after-label>}.json"
after="tools/bench-results/${2:?usage: tools/compare.sh <before-label> <after-label>}.json"

tools/.venv/bin/python3 src/benchmark/tools/compare.py benchmarks "$before" "$after"
