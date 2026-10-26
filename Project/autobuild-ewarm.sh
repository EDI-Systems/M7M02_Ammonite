#!/bin/bash
# autobuild-ewarm.sh - serial build of every ewarm-*.rvp (IAR toolchain).
# Run alongside autobuild-gccmf.sh / autobuild-rvmdk.sh in parallel.
cd "$(dirname "$0")"
CHAIN=ewarm
source ./autobuild-lib.sh
run_chain
