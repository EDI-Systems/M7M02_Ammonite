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

# ---- embedded resources (no Script/ folder dependency) -----------------------
VCP_PAIR_PS1="/tmp/ab_main_vcp_pair_$$.ps1"
WLINKE_CFG="/tmp/ab_main_wlinke_$$.cfg"
PROBE307_TCL="/tmp/ab_main_probe307_$$.tcl"

cat > "$VCP_PAIR_PS1" <<'PSEOF'
Get-CimInstance Win32_PnPEntity | Where-Object { $_.Name -match '\(COM\d+\)' } | ForEach-Object {
    $com = [regex]::Match($_.Name, '\((COM\d+)\)').Groups[1].Value
    $id = $_.PNPDeviceID
    if ($id -match 'VID_0483') {
        $parent = (Get-PnpDeviceProperty -InstanceId $id -KeyName 'DEVPKEY_Device_Parent' -ErrorAction SilentlyContinue).Data
        $sn = if ($parent) { ($parent -split '\\')[2] } else { '' }
        "STLINK $sn $com"
    } elseif ($id -match 'VID_1A86') {
        "WCHLINK $com"
    }
}
PSEOF

cat > "$WLINKE_CFG" <<'CFGEOF'
adapter driver wlinke
adapter speed 6000
transport select sdi
wlink_set_address 0x00000000
set _CHIPNAME wch_riscv
sdi newtap $_CHIPNAME cpu -irlen 5 -expected-id 0x00001
set _TARGETNAME $_CHIPNAME.cpu
target create $_TARGETNAME.0 wch_riscv -chain-position $_TARGETNAME
$_TARGETNAME.0 configure -work-area-phys 0x20000000 -work-area-size 10000 -work-area-backup 1
set _FLASHNAME $_CHIPNAME.flash
flash bank $_FLASHNAME wch_riscv 0x00000000 0 0 0 $_TARGETNAME.0
CFGEOF

printf 'init
shutdown
' > "$PROBE307_TCL"

trap 'rm -f "$VCP_PAIR_PS1" "$WLINKE_CFG" "$PROBE307_TCL"' EXIT

echo "== autobench $(date '+%F %T') =="

# --- optional probe: which boards are online? ---------------------------------
probe_opt() {   # probe_opt <chip> ; returns 0 if online
    local chip=$1
    local pairs line sn com out devid wantid ocd

    pairs=$(powershell -NoProfile -ExecutionPolicy Bypass \
                -File "$(cygpath -m "$VCP_PAIR_PS1")" 2>/dev/null) || return 1

    case "$chip" in
        v307)
            echo "$pairs" | grep -q '^WCHLINK' || return 1
            ocd=$("$OPENOCD" -f "$WLINKE_CFG" -f "$PROBE307_TCL" 2>&1) || true
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