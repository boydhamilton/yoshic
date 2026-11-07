#!/usr/bin/env bash
# Build and run the yoshic compiler benchmark suite (src/benchmark_game.cpp),
# saving results as labeled JSON under tools/bench-results/ so later runs can
# be diffed against earlier ones with tools/bench_compare.py.
#
# Usage:
#   tools/bench.sh <label>
#
# Example:
#   tools/bench.sh baseline
#   ... make an optimization ...
#   tools/bench.sh after-token-shrink
#   tools/bench_compare.py baseline after-token-shrink
set -euo pipefail
cd "$(dirname "$0")/.."

label="${1:?usage: tools/bench.sh <label>}"
mkdir -p tools/bench-results
out="tools/bench-results/${label}.json"

make benchmark

# --benchmark_repetitions + --benchmark_report_aggregates_only smooths over
# machine noise (the mean/median/stddev across N full repetitions is a much
# more trustworthy number to diff than one run's raw iteration count).
./benchmark_game \
    --benchmark_repetitions=7 \
    --benchmark_report_aggregates_only=true \
    --benchmark_format=json \
    --benchmark_out="$out"

echo "wrote $out"
