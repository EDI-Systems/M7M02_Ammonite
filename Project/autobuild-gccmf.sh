#!/bin/bash
# autobuild-gccmf.sh - serial build of every gccmf-*.rvp (GCC/Makefile).
# Run alongside autobuild-ewarm.sh / autobuild-rvmdk.sh in parallel.
cd "$(dirname "$0")"
CHAIN=gccmf
source ./autobuild-lib.sh
run_chain
