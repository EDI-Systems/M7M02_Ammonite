#!/bin/bash
# =============================================================================
# autobench.sh - bench phase main driver: probe which boards are connected,
# launch only the online ones in parallel, then summarize.
# Boards not connected are silently skipped (optional mode at this level);
# the per-chip scripts are strict once launched.
# =============================================================================
set -u
cd "$(dirname "$0")"

CUBEPROG="C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe"
OPENOCD="C:/MounRiver/MounRiver_Studio/toolchain/OpenOCD/bin/openocd.exe"

echo "== autobench $(date '+%F %T') =="

# --- optional probe: which boards are online? ---------------------------------
probe_opt() {   # probe_opt <chip> ; returns 0 if online
    local chip=$1
    local pairs line sn com out devid wantid ocd

    pairs=$(powershell -NoProfile -ExecutionPolicy Bypass \
                -File "$(cygpath -m Script/vcp_pair.ps1)" 2>/dev/null) || return 1

    case "$chip" in
        v307)
            echo "$pairs" | grep -q '^WCHLINK' || return 1
            ocd=$("$OPENOCD" -f Script/wlinke_v307.cfg -f Script/probe_v307.cmd 2>&1) || true
            echo "$ocd" | grep -q 'Examined RISC-V core' \
                && echo "$ocd" | grep -q 'XLEN=32' \
                && echo "$ocd" | grep -qi 'misa=0x40901125' \
                && return 0 || return 1
            ;;
        *)
            case "$chip" in
                f429) wantid=0x419 ;; g071) wantid=0x460 ;; h743) wantid=0x450 ;;
            esac
            while read -r line; do
                case "$line" in
                    STLINK\ *)
                        sn=${line#STLINK }; sn=${sn%% *}
                        out=$("$CUBEPROG" -c "port=SWD sn=$sn" 2>&1) || true
                        devid=$(echo "$out" | grep -oiE "Device ID[^0-9a-fx]*0x[0-9a-fA-F]+" \
                                | grep -oE '0x[0-9a-fA-F]+' | head -1)
                        [ "$devid" = "$wantid" ] && return 0
                        ;;
                esac
            done <<< "$pairs"
            return 1
            ;;
    esac
}

PIDS=""
for chip in f429 g071 h743 v307; do
    if probe_opt "$chip"; then
        echo "-- $chip online, launching..."
        bash "autobench-${chip}.sh" & PIDS="$PIDS $!"
    else
        echo "-- $chip not connected, skipping"
    fi
done

if [ -z "$PIDS" ]; then
    echo "FATAL: no boards online"; exit 1
fi

wait $PIDS

echo "============================================================"
for c in f429 g071 h743 v307; do
    [ -f "autobench_${c}.log" ] && tail -n 1 "autobench_${c}.log"
done
echo "done $(date '+%F %T')"