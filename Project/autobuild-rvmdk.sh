#!/bin/bash
# autobuild-rvmdk.sh - serial build of every rvmdk-*.rvp (Keil/UV4).
# Run alongside autobuild-ewarm.sh / autobuild-gccmf.sh in parallel.
cd "$(dirname "$0")"
CHAIN=rvmdk
source ./autobuild-lib.sh
run_chain
