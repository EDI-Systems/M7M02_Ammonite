#!/bin/bash
# autobench-g071.sh - bench every built G071 workspace on the G071 board (strict).
# Retry config - adjust as needed:
RETRY_DELAY=5    # seconds between flash/capture retries
RETRY_MAX=3      # max attempts

cd "$(dirname "$0")"
CHIP=g071
source ./autobench-lib.sh
chip_main g071