#!/bin/bash
# =============================================================================
# autobench-lib.sh - shared routines for autobench-<chip>.sh.
# The sourcing script must set CHIP (f429|g071|h743|v307) before sourcing.
# Output: autobench_<chip>.log in Project/ (2-line per rvp format).
# =============================================================================
set -u
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

CUBEPROG="C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe"
OPENOCD="C:/MounRiver/MounRiver_Studio/toolchain/OpenOCD/bin/openocd.exe"

LOG="autobench_${CHIP}.log"
CAP_SECONDS=92
FLASH_LOCK="$PROJECT_DIR/.autobench_flash.lock"

SN="-"; COMPORT=""; FLASHER=""

die() { echo "FATAL: $*" >> "$LOG"; exit 1; }

# ---- board detection ---------------------------------------------------------
chip2devid() {
    case "$1" in
        f429) echo 0x419 ;; g071) echo 0x460 ;;
        h743) echo 0x450 ;; v307) echo "" ;;
    esac
}

chip2wspat() {
    case "$1" in
        f429) echo '*STM32F429ZIT6*' ;; g071) echo '*STM32G071RBT6*' ;;
        h743) echo '*STM32H743ZIT6*' ;; v307) echo '*CH32V307VCT6*' ;;
    esac
}

# probe_chip <chip> [optional] - finds SN/COM for this chip; returns 1 if absent
probe_chip() {
    local want=$1 opt=${2:-}
    local pairs line sn com out devid wantid ocd

    pairs=$(powershell -NoProfile -ExecutionPolicy Bypass \
                -File "$(cygpath -m Script/vcp_pair.ps1)" 2>/dev/null) \
        || { [ -n "$opt" ] && return 1; die "cannot enumerate USB serial devices"; }

    case "$want" in
        v307)
            while read -r line; do
                case "$line" in
                    WCHLINK\ *)
                        COMPORT=$(echo "$line" | awk '{print $NF}')
                        ocd=$("$OPENOCD" -f Script/wlinke_v307.cfg -f Script/probe_v307.cmd 2>&1) || true
                        echo "$ocd" | grep -q 'Examined RISC-V core' \
                            && echo "$ocd" | grep -q 'XLEN=32' \
                            && echo "$ocd" | grep -qi 'misa=0x40901125' \
                            || { [ -n "$opt" ] && return 1; die "WCH-Link present but no CH32V307"; }
                        FLASHER=wch; return 0 ;;
                esac
            done <<< "$pairs"
            ;;
        *)
            wantid=$(chip2devid "$want")
            while read -r line; do
                case "$line" in
                    STLINK\ *)
                        sn=${line#STLINK }; sn=${sn%% *}
                        com=$(echo "$line" | awk '{print $NF}')
                        out=$("$CUBEPROG" -c "port=SWD sn=$sn" 2>&1) || true
                        devid=$(echo "$out" | grep -oiE "Device ID[^0-9a-fx]*0x[0-9a-fA-F]+" \
                                | grep -oE '0x[0-9a-fA-F]+' | head -1)
                        if [ "$devid" = "$wantid" ]; then
                            SN=$sn; COMPORT=$com; FLASHER=st; return 0
                        fi
                        ;;
                esac
            done <<< "$pairs"
            ;;
    esac
    return 1
}

# ---- scan bins ---------------------------------------------------------------
scan_bins() {   # echoes "<ws>|<bin>" for each workspace with a built bin
    local pat=$(chip2wspat "$CHIP")
    local ws bin
    for ws in $(ls -d ${pat} 2>/dev/null); do
        for bin in "$ws/Kernel/Project/Object/kernel.bin" \
                   "$ws/Kernel/Project/Debug/Output/Kernel.bin" \
                   "$ws/Kernel/Project/kernel_keil.bin"; do
            [ -f "$bin" ] && echo "${ws}|$bin" && break
        done
    done
}

# ---- flash + capture + judge --------------------------------------------------
flash_st() {
    local sn=$1 bin=$2 out
    (
        flock 9
        out=$("$CUBEPROG" -c "port=SWD sn=$sn" -w "$bin" 0x08000000 -v -rst 2>&1)
        echo "$out" > /tmp/flashst_$$
    ) 9>"$FLASH_LOCK"
    out=$(cat /tmp/flashst_$$ 2>/dev/null)
    echo "$out" | grep -qiE 'verified|download verified successfully' \
        || { echo "flash failed"; return 1; }
}

flash_wch() {
    local bin=$1 win log
    win=$(cygpath -m "$bin" 2>/dev/null || echo "$bin")
    log=/tmp/flashwch_$$
    cat > /tmp/flashwch_$$.cmd <<EOF
flash write_image erase $win 0x00000000
echo "FLASH_WRITE_DONE"
verify_image $win 0x00000000
echo "FLASH_VERIFY_DONE"
reset run
shutdown
EOF
    "$OPENOCD" -f Script/wlinke_v307.cfg -f /tmp/flashwch_$$.cmd > "$log" 2>&1
    if ! grep -q 'FLASH_VERIFY_DONE' "$log" || grep -q 'Error:' "$log"; then
        return 1
    fi
    rm -f /tmp/flashwch_$$.cmd "$log"
}

capture() {   # capture <com> <secs> <outfile>
    local com=$1 secs=$2 out=$3 winout
    winout=$(cygpath -m "$out" 2>/dev/null || echo "$out")
    cat > /tmp/cap_$$.ps1 <<EOF
\$sp = New-Object System.IO.Ports.SerialPort('$com', 115200, 'None', 8, 'One')
\$sp.Open()
Start-Sleep -Milliseconds 400
\$sp.DiscardInBuffer()
\$sw = [Diagnostics.Stopwatch]::StartNew()
\$sb = New-Object Text.StringBuilder
while (\$sw.Elapsed.TotalSeconds -lt $secs) {
    \$sb.Append(\$sp.ReadExisting()) | Out-Null
    Start-Sleep -Milliseconds 50
}
\$sp.Close()
[IO.File]::WriteAllText('WINOUTPLACEHOLDER', \$sb.ToString())
EOF
    sed -i "s|WINOUTPLACEHOLDER|$winout|" /tmp/cap_$$.ps1
    powershell -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -m /tmp/cap_$$.ps1)" \
        || { echo "capture failed"; return 1; }
    rm -f /tmp/cap_$$.ps1
    [ -s "$out" ] || { echo "capture empty"; return 1; }
}

judge() {   # judge <cap> ; echoes fault count; returns 0 iff 0 faults + has Yield
    local cap=$1 f
    f=$(grep -cE 'Recoverable fault|HardFault|INVSTATE|assert fail' "$cap" 2>/dev/null)
    f=${f:-0}
    echo "$f"
    [ "$f" = "0" ] && grep -q '^Yield' "$cap"
}


# ---- per-rvp bench ------------------------------------------------------------
# Retry config - override in autobench-<chip>.sh BEFORE sourcing this lib.
RETRY_DELAY=${RETRY_DELAY:-5}       # seconds between retries
RETRY_MAX=${RETRY_MAX:-3}           # max attempts for flash/capture

bench_one() {   # bench_one <ws> <bin> ; echoes result lines, returns 0/1
    local ws=$1 bin=$2
    local t0 el attempt flashrc cap faults nitems

    # ---- flash (with retry) ----
    t0=$SECONDS
    flashrc=1
    for attempt in $(seq 1 "$RETRY_MAX"); do
        flashrc=0
        case "$FLASHER" in
            st)  flash_st  "$SN" "$bin" || flashrc=1 ;;
            wch) flash_wch "$bin"      || flashrc=1 ;;
        esac
        [ "$flashrc" -eq 0 ] && break
        if [ "$attempt" -lt "$RETRY_MAX" ]; then
            echo "   flash retry $attempt/$RETRY_MAX (waiting ${RETRY_DELAY}s)..."
            sleep "$RETRY_DELAY"
        fi
    done
    el=$((SECONDS-t0))
    if [ "$flashrc" -ne 0 ]; then
        echo "   FAILED: flash error after $RETRY_MAX attempts (${el}s)"
        return 1
    fi
    echo "   flash OK (${el}s)"

    # ---- capture (with retry) ----
    t0=$SECONDS
    cap="/tmp/bench_${CHIP}_$$.txt"
    rm -f "$cap"
    for attempt in $(seq 1 "$RETRY_MAX"); do
        if capture "$COMPORT" "$CAP_SECONDS" "$cap"; then
            break
        fi
        if [ "$attempt" -lt "$RETRY_MAX" ]; then
            echo "   capture retry $attempt/$RETRY_MAX (waiting ${RETRY_DELAY}s)..."
            sleep "$RETRY_DELAY"
        fi
    done
    el=$((SECONDS-t0))
    if [ ! -s "$cap" ]; then
        echo "   FAILED: capture empty after $RETRY_MAX attempts (${el}s)"
        return 1
    fi
    echo "   capture OK (${el}s)"

    # ---- bin size ----
    local binsz
    binsz=$(stat -c%s "$bin" 2>/dev/null || wc -c < "$bin" 2>/dev/null || echo 0)
    echo "   bin size: ${binsz} bytes"

    # ---- judge + data ----
    faults=$(judge "$cap")
    if [ "$faults" = "0" ] && grep -q '^Yield' "$cap"; then
        nitems=$(grep -cE '^(Yield|Notification|Semaphore|Mailbox|Message|Memory|ISR)' "$cap" 2>/dev/null)
        nitems=${nitems:-0}
        echo "   PASS: 0 faults, ${nitems} items"
        # benchmark data lines
        grep -E '^(Yield|Notification|Semaphore|Mailbox|Message|Memory|ISR)' "$cap" 2>/dev/null | head -20
        return 0
    else
        echo "   FAILED: ${faults:-?} faults"
        # first few lines for diagnosis
        head -5 "$cap" 2>/dev/null | sed 's/^/   | /'
        return 1
    fi
}

# ---- chip main -----------------------------------------------------------------
chip_main() {   # chip_main <chip>
    local chip=$1
    local pairs ws bin total=0 pass=0

    : > "$LOG"

    echo "== bench $chip $(date '+%F %T') ==" > "$LOG"

    # strict probe - the board MUST be here (autobench.sh already filtered)
    probe_chip "$chip" \
        || die "board '$chip' not found - autobench.sh should have filtered this"

    # scan bins
    pairs=$(scan_bins)
    if [ -z "$pairs" ]; then
        echo "   (no built workspaces for $chip)" >> "$LOG"
        echo "== $chip BENCH SUMMARY: 0/0 (nothing to test) ==" >> "$LOG"
        return 0
    fi

    # bench each
    while IFS='|' read -r ws bin; do
        [ -z "$ws" ] && continue
        total=$((total+1))
        echo ">> TEST $ws" >> "$LOG"
        if bench_one "$ws" "$bin" >> "$LOG" 2>&1; then
            pass=$((pass+1))
        fi
    done <<< "$pairs"

    echo "============================================================" >> "$LOG"
    echo "== $chip BENCH SUMMARY: $pass/$total passed ==" >> "$LOG"

    [ "$pass" -eq "$total" ]
}
