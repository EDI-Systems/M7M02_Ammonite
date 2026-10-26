#!/bin/bash
# autobench-h743.sh - bench every built H743 workspace on the H743 board (strict).
# Retry config - adjust as needed:
RETRY_DELAY=5    # seconds between flash/capture retries
RETRY_MAX=3      # max attempts

cd "$(dirname "$0")"
CHIP=h743
source ./autobench-lib.sh
chip_main h743