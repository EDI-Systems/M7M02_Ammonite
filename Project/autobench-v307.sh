#!/bin/bash
# autobench-v307.sh - bench every built V307 workspace on the V307 board (strict).
# Retry config - adjust as needed:
RETRY_DELAY=5    # seconds between flash/capture retries
RETRY_MAX=3      # max attempts

cd "$(dirname "$0")"
CHIP=v307
source ./autobench-lib.sh
chip_main v307