#!/bin/bash
# =============================================================================
# autobuild-lib.sh - shared routines for autobuild-<chain>.sh.
# Each chain script builds its own rvp subset serially; different chains may
# run in parallel (IAR/UV4 races only occur within one toolchain).
# The sourcing script must set CHAIN (ewarm|gccmf|rvmdk) before sourcing.
# Output: one file per chain - autobuild-<chain>.log in Project/ - carrying
# both the human-readable build trace and the machine-readable manifest
# lines ("OK|board|rvp|ws|bin" / "FAIL|board|rvp|ws|").
# =============================================================================
set -u
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

IARBUILD="C:/Program Files/IAR Systems/Embedded Workbench 9.2/common/bin/iarbuild.exe"
UV4="C:/Keil_v5_ARM/UV4/UV4.exe"
FROMELF="C:/Keil_v5_ARM/ARM/ARMCLANG/bin/fromelf.exe"

# ---- error/warning scanning (case-insensitive) -------------------------------
# log_err_count <logfile> <chain> - prints number of real errors (0 if clean)
log_err_count() {
    local log=$1 n=0
    [ -f "$log" ] || { echo 0; return; }
    case "$2" in
    gccmf)
        # gcc/ld diagnostics ("error:", "fatal error:") and make failures ("*** [x] Error N")
        n=$(grep -icE 'error:|fatal error|\*\*\*.*error [1-9]' "$log") ;;
    ewarm)
        # IAR diagnostics "Error[Pe###]:" plus nonzero error summary
        n=$(( $(grep -icE 'error\[[^]]*\]' "$log") \
            + $(grep -icE '^error:' "$log") \
            + $(grep -icE 'total number of errors: *[1-9][0-9]*' "$log") )) ;;
    rvmdk)
        # Keil/armclang diagnostics "error:" plus nonzero "N Error(s)" summary
        # ("[1-9][0-9]* error" deliberately does not match "0 Error(s)")
        n=$(( $(grep -icE 'error:' "$log") \
            + $(grep -icE '[1-9][0-9]* *error\(' "$log") )) ;;
    esac
    echo "$n"
}

# log_warn_count <logfile> <chain> - prints number of warnings (summary-aware)
log_warn_count() {
    local log=$1 n=0
    [ -f "$log" ] || { echo 0; return; }
    case "$2" in
    gccmf)
        n=$(grep -icE 'warning:' "$log") ;;
    ewarm)
        # prefer the IAR summary number; fall back to diagnostic line count
        n=$(grep -ioE 'total number of warnings: *[0-9]+' "$log" | grep -oE '[0-9]+$' | awk '{s+=$1} END{print s+0}')
        [ "$n" -eq 0 ] && n=$(grep -icE 'warning\[[^]]*\]' "$log") ;;
    rvmdk)
        # summary "N Error(s), M Warning(s)" - sum the M values
        n=$(grep -ioE '[0-9]+ *warning' "$log" | grep -oE '^[0-9]+' | awk '{s+=$1} END{print s+0}')
        [ "$n" -eq 0 ] && n=$(grep -icE 'warning:' "$log") ;;
    esac
    echo "$n"
}

BUILD_WARN=0   # accumulated warnings for the current build_one() invocation


LOG="autobuild_${CHAIN}.log"
: > "$LOG"

rvp2board() {
    case "$1" in
        *stm32f429*) echo f429 ;; *stm32g071*) echo g071 ;;
        *stm32h743*) echo h743 ;; *stm32f767*) echo f767 ;;
        *stm32f405*) echo f405 ;; *stm32l071*) echo l071 ;;
        *ch32v307*)  echo v307 ;; *)           echo unknown ;;
    esac
}

build_gcc() {
    local ws=$1 m lg err
    BUILD_WARN=0
    for m in "Monitor/Project/monitor" "Proc1/Project/prc_proc1" \
             "Proc2/Project/prc_proc2" "Virt1/Project/prc_virt1" \
             "Kernel/Project/kernel"; do
        ( cd "$ws/$(dirname "$m")" && make -j8 -f "$(basename "$m")" ) \
            > "$ws/build_$(basename "$m").log" 2>&1
        lg="$ws/build_$(basename "$m").log"
        err=$(log_err_count "$lg" gccmf)
        if [ "$err" -gt 0 ]; then
            echo "gcc failed at $m ($err errors):"
            grep -iE 'error:|fatal error|\*\*\*.*error [1-9]' "$lg" | head -5
            return 1
        fi
        BUILD_WARN=$((BUILD_WARN + $(log_warn_count "$lg" gccmf)))
    done
    [ -f "$ws/Kernel/Project/Object/kernel.bin" ]
}

build_iar() {
    local ws=$1 n proj ewp lg err
    BUILD_WARN=0
    for n in "Virt1/prc_virt1" "Monitor/monitor" "Proc1/prc_proc1" \
             "Proc2/prc_proc2" "Kernel/kernel"; do
        proj="${n%%/*}"; ewp="${n##*/}"
        "$IARBUILD" "$ws/$proj/Project/$ewp.ewp" -build Debug \
            > "$ws/build_$ewp.log" 2>&1
        lg="$ws/build_$ewp.log"
        err=$(log_err_count "$lg" ewarm)
        if [ "$err" -gt 0 ]; then
            echo "iar failed at $ewp ($err errors):"
            grep -iE 'error\[|^error:|total number of errors: *[1-9]' "$lg" | head -5
            return 1
        fi
        BUILD_WARN=$((BUILD_WARN + $(log_warn_count "$lg" ewarm)))
    done
    [ -f "$ws/Kernel/Project/Debug/Output/Kernel.bin" ]
}

build_keil() {
    local ws=$1 m dir axf lg err
    BUILD_WARN=0
    for m in "Monitor/monitor" "Proc1/prc_proc1" "Proc2/prc_proc2" \
             "Virt1/prc_virt1" "Kernel/kernel"; do
        dir="$ws/${m%%/*}/Project"
        ( cd "$dir" && "$UV4" -b "${m##*/}.uvprojx" -j0 -o build.log ) >/dev/null 2>&1
        lg="$dir/build.log"
        err=$(log_err_count "$lg" rvmdk)
        if [ "$err" -gt 0 ]; then
            echo "keil failed at ${m##*/} ($err errors):"
            grep -iE 'error:|[1-9][0-9]* *error\(' "$lg" | head -5
            return 1
        fi
        BUILD_WARN=$((BUILD_WARN + $(log_warn_count "$lg" rvmdk)))
    done
    axf=$(find "$ws/Kernel/Project" -name '*.axf' | head -1)
    [ -n "$axf" ] || { echo "keil axf missing"; return 1; }
    "$FROMELF" --bincombined --output "$ws/Kernel/Project/kernel_keil.bin" "$axf"
}

build_one() {   # echoes the bin path on success
    local rvp=$1
    local ws=$(echo "${rvp%.rvp}" | tr '[:lower:]' '[:upper:]')
    local attempt rc

    for attempt in 1 2; do
        rc=0
        case "$CHAIN" in
            gccmf) build_gcc  "$ws" || rc=1 ;;
            ewarm) build_iar  "$ws" || rc=1 ;;
            rvmdk) build_keil "$ws" || rc=1 ;;
        esac
        [ "$rc" -eq 0 ] && break
        echo "   retry $attempt..." | tee -a "$LOG"
    done
    [ "$rc" -ne 0 ] && return 1

    case "$CHAIN" in
        gccmf) echo "$ws/Kernel/Project/Object/kernel.bin" ;;
        ewarm) echo "$ws/Kernel/Project/Debug/Output/Kernel.bin" ;;
        rvmdk) echo "$ws/Kernel/Project/kernel_keil.bin" ;;
    esac
}

run_chain() {   # serially build every "$CHAIN"-*.rvp; returns 0 iff all OK
    local rvp base ws genrc t0 bin el
    local total=0 ok=0
    echo "== $CHAIN build $(date '+%F %T') ==" > "$LOG"

    for rvp in $(ls "$CHAIN"-*.rvp 2>/dev/null | sort); do
        total=$((total+1))
        base=${rvp%.rvp}
        ws=$(echo "$base" | tr '[:lower:]' '[:upper:]')
        echo ">> BUILD $rvp" >> "$LOG"

        ./benchmark.sh "$rvp" > /dev/null 2>&1
        genrc=$?
        cp log.txt "$ws.gen.log" 2>/dev/null; rm -f log.txt report.txt
        # case-insensitive error scan of the generator log
        if [ "$genrc" -ne 0 ] || grep -qiE '(^|[^a-z])error' "$ws.gen.log" 2>/dev/null; then
            echo "   FAILED: generation error" >> "$LOG"
            grep -inE '(^|[^a-z])error' "$ws.gen.log" 2>/dev/null | head -3 | sed 's/^/      /' >> "$LOG"
            rm -f "$ws.gen.log"
            continue
        fi
        genwarn=$(grep -icE '(^|[^a-z])warning' "$ws.gen.log" 2>/dev/null)
        [ "${genwarn:-0}" -gt 0 ] && echo "   gen warnings: $genwarn" >> "$LOG"
        rm -f "$ws.gen.log"

        t0=$SECONDS
        bin=$(build_one "$rvp")
        el=$((SECONDS-t0))
        if [ -n "$bin" ] && [ -f "$bin" ]; then
            ok=$((ok+1))
            echo "   OK (${el}s): $bin" >> "$LOG"
            [ "${BUILD_WARN:-0}" -gt 0 ] && echo "   warnings: $BUILD_WARN" >> "$LOG"
        else
            echo "   FAILED (${el}s): $bin" >> "$LOG"
        fi
    done

    echo "============================================================" >> "$LOG"
    echo "== $CHAIN BUILD SUMMARY: $ok/$total built ==" >> "$LOG"
    [ "$ok" -eq "$total" ] && [ "$total" -gt 0 ]
}
