#!/usr/bin/env python3
"""Compare two labeled benchmark runs produced by tools/bench.sh.

Google Benchmark ships its own tools/compare.py, but it hard-imports scipy
(for the Mann-Whitney U test) which isn't installed here and isn't worth
pulling in just to eyeball a percentage change. This does the same basic
job - match benchmarks by name across two JSON result files and report the
percent change in real_time/cpu_time - using only the standard library.

Usage:
    tools/bench_compare.py <before-label> <after-label>
    tools/bench_compare.py tools/bench-results/before.json tools/bench-results/after.json
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def resolve(label_or_path: str) -> Path:
    p = Path(label_or_path)
    if p.exists():
        return p
    p = ROOT / "tools" / "bench-results" / f"{label_or_path}.json"
    if p.exists():
        return p
    sys.exit(f"error: no results file for '{label_or_path}' (looked for {p})")


def load(path: Path) -> dict:
    with open(path) as f:
        data = json.load(f)
    # keep only the aggregate rows (mean/median/stddev), skip raw per-rep rows
    out = {}
    for b in data["benchmarks"]:
        if b.get("run_type") == "aggregate" and b.get("aggregate_name") == "mean":
            out[b["name"].removesuffix("_mean")] = b
        elif "run_type" not in b:  # non-repeated run, one row per benchmark
            out[b["name"]] = b
    return out


def pct(before: float, after: float) -> float:
    return (after - before) / before * 100.0 if before else float("nan")


def main() -> None:
    if len(sys.argv) != 3:
        sys.exit(__doc__)

    before = load(resolve(sys.argv[1]))
    after = load(resolve(sys.argv[2]))

    names = [n for n in before if n in after]
    missing_before = [n for n in after if n not in before]
    missing_after = [n for n in before if n not in after]

    name_w = max((len(n) for n in names), default=20) + 2
    header = f"{'benchmark':<{name_w}}{'before (ns)':>14}{'after (ns)':>14}{'delta':>10}"
    print(header)
    print("-" * len(header))
    for n in names:
        b_time = before[n]["real_time"]
        a_time = after[n]["real_time"]
        d = pct(b_time, a_time)
        marker = "  <-- faster" if d <= -3 else ("  <-- slower" if d >= 3 else "")
        print(f"{n:<{name_w}}{b_time:>14.1f}{a_time:>14.1f}{d:>+9.1f}%{marker}")

    if missing_before:
        print(f"\nonly in 'after': {', '.join(missing_before)}")
    if missing_after:
        print(f"only in 'before': {', '.join(missing_after)}")


if __name__ == "__main__":
    main()
