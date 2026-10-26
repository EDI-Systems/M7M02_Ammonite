#!/bin/bash
# =============================================================================
# autotest-build.sh - phase 1: serial build of every rvp of every connected
# board. No parallel compilation (one build at a time - eliminates IAR/UV4
# postbuild and license races). Artifacts are KEPT for phase 2; the manifest
# at Script/autotest_build_manifest.txt lists "OK|board|rvp|ws|bin" lines.
# =============================================================================
set -u
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_DIR"

CUBEPROG="C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe"
OPENOCD="C:/MounRiver/MounRiver_Studio/toolchain/OpenOCD/bin/openocd.exe"
IARBUILD="C:/Program Files/IAR Systems/Embedded Workbench 9.2/common/bin/iarbuild.exe"
UV4="C:/Keil_v5_ARM/UV4/UV4.exe"
FROMELF="C:/Keil_v5_ARM/ARM/ARMCLANG/bin/fromelf.exe"

MANIFEST="Script/autotest_build_manifest.txt"
LOG="Script/autotest_build_log.txt"
: > "$MANIFEST"
: > "$LOG"

die() { echo "FATAL: $*" | tee -a "$LOG"; exit 1; }

id2board() {
    case "$1" in
        0x419) echo f429 ;; 0x460) echo g071 ;; 0x450) echo h743 ;;
        0x451) echo f767 ;; 0x413) echo f405 ;; *) echo "" ;;
    esac
}

board2rvp() {
    case "$1" in
        f429) echo '*stm32f429*' ;; g071) echo '*stm32g071*' ;;
        h743) echo '*stm32h743*' ;; f767) echo '*stm32f767*' ;;
        f405) echo '*stm32f405*' ;; v307) echo '*ch32v307*' ;;
    esac
}

detect_all() {
    local pairs line sn com out devid board ocd
    pairs=$(powershell -NoProfile -ExecutionPolicy Bypass \
                -File "$(cygpath -m Script/vcp_pair.ps1)" 2>/dev/null) \
        || die "cannot enumerate USB serial devices"
    [ -n "$pairs" ] || die "no USB serial devices found"

    while read -r line; do
        [ -z "$line" ] && continue
        com=$(echo "$line" | awk '{print $NF}')
        case "$line" in
            STLINK\ *)
                sn=${line#STLINK }; sn=${sn%% *}
                out=$("$CUBEPROG" -c "port=SWD sn=$sn" 2>&1) || true
                devid=$(echo "$out" | grep -oiE 'Device ID[^0-9a-fx]*0x[0-9a-fA-F]+' \
                        | grep -oE '0x[0-9a-fA-F]+' | head -1)
                board=$(id2board "$devid")
                [ -n "$board" ] || die "unknown Device ID '$devid' on ST-Link ...${sn: -6}"
                echo "-- board $board: sn ...${sn: -6}, $com (id $devid)" | tee -a "$LOG"
                BOARD_SPECS+="$board|$sn|$com"$'\n'
                ;;
            WCHLINK\ *)
                ocd=$("$OPENOCD" -f Script/wlinke_v307.cfg -f Script/probe_v307.cmd 2>&1) || true
                echo "$ocd" | grep -q 'Examined RISC-V core' \
                    && echo "$ocd" | grep -q 'XLEN=32' \
                    && echo "$ocd" | grep -qi 'misa=0x40901125' \
                    || die "WCH-Link present but no CH32V307 core"
                echo "-- board v307: WCH-Link, $com (RV32 ok)" | tee -a "$LOG"
                BOARD_SPECS+="v307|-$com"$'\n'
                ;;
        esac
    done <<< "$pairs"
    [ -n "$BOARD_SPECS" ] || die "no boards paired"
}

build_gcc() {
    local ws=$1 m
    for m in "Monitor/Project/monitor" "Proc1/Project/prc_proc1" \
             "Proc2/Project/prc_proc2" "Virt1/Project/prc_virt1" \
             "Kernel/Project/kernel"; do
        ( cd "$ws/$(dirname "$m")" && make -j8 -f "$(basename "$m")" ) \
            > "$ws/build_$(basename "$m").log" 2>&1 \
            || { echo "gcc failed at $m"; return 1; }
    done
    [ -f "$ws/Kernel/Project/Object/kernel.bin" ]
}

build_iar() {
    local ws=$1 n
    for n in "Virt1/prc_virt1" "Monitor/monitor" "Proc1/prc_proc1" \
             "Proc2/prc_proc2" "Kernel/kernel"; do
        local proj="${n%%/*}" ewp="${n##*/}"
        "$IARBUILD" "$ws/$proj/Project/$ewp.ewp" -build Debug \
            > "$ws/build_$ewp.log" 2>&1
        grep -q "Total number of errors: 0" "$ws/build_$ewp.log" \
            || { echo "iar failed at $ewp"; return 1; }
    done
    [ -f "$ws/Kernel/Project/Debug/Output/Kernel.bin" ]
}

build_keil() {
    local ws=$1 m dir axf
    for m in "Monitor/monitor" "Proc1/prc_proc1" "Proc2/prc_proc2" \
             "Virt1/prc_virt1" "Kernel/kernel"; do
        dir="$ws/${m%%/*}/Project"
        ( cd "$dir" && "$UV4" -b "${m##*/}.uvprojx" -j0 -o build.log ) >/dev/null 2>&1
        grep -q '0 Error' "$dir/build.log" 2>/dev/null \
            || { echo "keil failed at ${m##*/}"; return 1; }
    done
    axf=$(find "$ws/Kernel/Project" -name '*.axf' | head -1)
    [ -n "$axf" ] || { echo "keil axf missing"; return 1; }
    "$FROMELF" --bincombined --output "$ws/Kernel/Project/kernel_keil.bin" "$axf"
}

build_one() {   # build_one <rvp> -> echoes bin path on success
    local rvp=$1
    local base=${rvp%.rvp}
    local ws=$(echo "$base" | tr '[:lower:]' '[:upper:]')
    local chain=${base%%-*}
    local attempt rc

    for attempt in 1 2; do
        rc=0
        case "$chain" in
            gccmf) build_gcc  "$ws" || rc=1 ;;
            ewarm) build_iar  "$ws" || rc=1 ;;
            rvmdk) build_keil "$ws" || rc=1 ;;
        esac
        [ "$rc" -eq 0 ] && break
        echo "   retry $attempt..." | tee -a "$LOG"
    done
    [ "$rc" -ne 0 ] && return 1

    case "$chain" in
        gccmf) echo "$ws/Kernel/Project/Object/kernel.bin" ;;
        ewarm) echo "$ws/Kernel/Project/Debug/Output/Kernel.bin" ;;
        rvmdk) echo "$ws/Kernel/Project/kernel_keil.bin" ;;
    esac
}


# ---- main --------------------------------------------------------------------
echo "== build phase $(date '+%Y-%m-%d %H:%M:%S') ==" | tee -a "$LOG"
detect_all

TOTAL=0; OK=0
while read -r spec; do
    [ -z "$spec" ] && continue
    board=${spec%%|*}
    pat=$(board2rvp "$board")
    for rvp in $(ls $pat.rvp 2>/dev/null); do
        TOTAL=$((TOTAL+1))
        base=${rvp%.rvp}
        ws=$(echo "$base" | tr '[:lower:]' '[:upper:]')
        echo "------------------------------------------------------------" | tee -a "$LOG"
        echo ">> BUILD $rvp" | tee -a "$LOG"

        # generate (this script is serial, so no lock needed)
        ./benchmark.sh "$rvp" > /dev/null 2>&1
        genrc=$?
        cp log.txt "$ws.gen.log" 2>/dev/null; rm -f log.txt report.txt
        if [ "$genrc" -ne 0 ] || grep -q '^Error:' "$ws.gen.log" 2>/dev/null; then
            echo "   FAIL: generation" | tee -a "$LOG"
            echo "FAIL|$board|$rvp|$ws|" >> "$MANIFEST"
            continue
        fi
        rm -f "$ws.gen.log"

        t0=$SECONDS
        bin=$(build_one "$rvp")
        if [ -n "$bin" ] && [ -f "$bin" ]; then
            OK=$((OK+1))
            echo "   OK ($((SECONDS-t0))s) -> $bin" | tee -a "$LOG"
            echo "OK|$board|$rvp|$ws|$bin" >> "$MANIFEST"
        else
            echo "   FAIL: build" | tee -a "$LOG"
            echo "FAIL|$board|$rvp|$ws|" >> "$MANIFEST"
        fi
    done
done <<< "$BOARD_SPECS"

echo "============================================================" | tee -a "$LOG"
echo "== BUILD SUMMARY: $OK/$TOTAL built ==" | tee -a "$LOG"
echo "manifest: $MANIFEST"
exit $((TOTAL-OK))

