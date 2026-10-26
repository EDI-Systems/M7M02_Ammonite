#!/bin/bash
# autobench-f429.sh - bench every built F429 workspace on the F429 board (strict).
# Retry config - adjust as needed:
RETRY_DELAY=5    # seconds between flash/capture retries
RETRY_MAX=3      # max attempts

cd "$(dirname "$0")"
CHIP=f429
source ./autobench-lib.sh
chip_main f429