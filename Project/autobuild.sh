#!/bin/bash
# =============================================================================
# autobuild.sh - build phase main driver: launch the three per-toolchain
# builders in parallel (each chain is serial internally), then summarize.
# Each chain writes autobuild-<chain>.log in this folder; no other output
# files are produced.
# =============================================================================
set -u
cd "$(dirname "$0")"

echo "== autobuild (3 chains parallel, each serial internally) $(date '+%F %T') =="

bash autobuild-ewarm.sh  & P1=$!
bash autobuild-gccmf.sh  & P2=$!
bash autobuild-rvmdk.sh  & P3=$!

echo "ewarm pid=$P1  gccmf pid=$P2  rvmdk pid=$P3"
wait $P1 $P2 $P3

echo "============================================================"
for c in ewarm gccmf rvmdk; do
    tail -n 1 "autobuild_${c}.log" 2>/dev/null
done
echo "done $(date '+%F %T')"
