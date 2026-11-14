#!/bin/sh

# This script removes every subdirectory of the current directory.  Refuse to
# run from anywhere else: from the repository root it would wipe the tree.
if [ ! -f update-maintainer-only.sh ]; then
    echo "error: run this script from the Guest/ directory" >&2
    exit 1
fi

SRC_PREFIX="../../"
DST_PREFIX="./"

# FreeRTOS V9.00
GUESTS="FreeRTOS_V900/portable/GCC/ARM_A6M_RVM "
GUESTS+="FreeRTOS_V900/portable/GCC/ARM_A7M_RVM "
GUESTS+="FreeRTOS_V900/portable/GCC/RISC-V_RV32P_RVM "
GUESTS+="FreeRTOS_V900/portable/RVDS/ARM_A6M_RVM "
GUESTS+="FreeRTOS_V900/portable/RVDS/ARM_A7M_RVM "
GUESTS+="FreeRTOS_V900/portable/IAR/ARM_A6M_RVM "
GUESTS+="FreeRTOS_V900/portable/IAR/ARM_A7M_RVM "
GUESTS+="FreeRTOS_V900/test "

# uC/OS III V3.08, uC/CPU V1.32, uC-LIB V1.39
GUESTS+="uCOSIII_V308/uC-OS3/Ports/ARM-Cortex-M/ARMv6-M-RVM "
GUESTS+="uCOSIII_V308/uC-CPU/ARM-Cortex-M/ARMv6-M-RVM "
GUESTS+="uCOSIII_V308/uC-OS3/Ports/ARM-Cortex-M/ARMv7-M-RVM "
GUESTS+="uCOSIII_V308/uC-CPU/ARM-Cortex-M/ARMv7-M-RVM "
GUESTS+="uCOSIII_V308/uC-OS3/Ports/RISC-V/RV32-RVM "
GUESTS+="uCOSIII_V308/uC-CPU/RISC-V-RVM "
GUESTS+="uCOSIII_V308/uC-OS3/Test "

# RT-Thread V4.1.1
GUESTS+="RTThread_V411/libcpu/arm/armv6m_rvm "
GUESTS+="RTThread_V411/libcpu/arm/armv7m_rvm "
GUESTS+="RTThread_V411/libcpu/risc-v/rv32p_rvm "
GUESTS+="RTThread_V411/test "

# DjyOS V1.1.0
# The port layer lives under the arch tree; the benchmark files sit in the
# distribution's own test/ directory, so they are mirrored one by one (a
# whole-directory entry would drag the author's example tests along).
GUESTS+="DjyOS_V110/bsp/arch/arm/cortex-m/armv7e-m_rvm "
GUESTS+="DjyOS_V110/test/djy_board.c "
GUESTS+="DjyOS_V110/test/djy_benchmark.c "
GUESTS+="DjyOS_V110/test/djy_main.c "
GUESTS+="DjyOS_V110/test/chip/djy_test_stm32h743zi_rvm.h "

# KLite V5.2.1
GUESTS+="KLite_V521/sources/cpu/armv7-m_rvm "
GUESTS+="KLite_V521/test "

# ChibiOS 21.11 LTS (CHB)
# The port is a new architecture-level directory; the benchmark and the
# per-VM chconf template are ours.  The rest of the tree is the vendor
# distribution and is not mirrored.
GUESTS+="ChibiOS_V2111/os/common/ports/ARMv7-M-RVM "
GUESTS+="ChibiOS_V2111/os/common/ports/ARMv6-M-RVM "
GUESTS+="ChibiOS_V2111/os/common/ports/RV32P-RVM "
GUESTS+="ChibiOS_V2111/test/chb_benchmark.c "
GUESTS+="ChibiOS_V2111/test/chb_main.c "
GUESTS+="ChibiOS_V2111/test/chip/chb_test_stm32h743zi_rvm.h "
GUESTS+="ChibiOS_V2111/test/chip/chb_test_stm32g071rb_rvm.h "
GUESTS+="ChibiOS_V2111/test/chip/chb_test_stm32f429zi_rvm.h "
GUESTS+="ChibiOS_V2111/test/chip/chb_test_ch32v307vc_rvm.h "

# uC/OS-II V2.93
# The ports follow the official Ports/ tree layout; the test directory is ours.
GUESTS+="uCOSII_V293/Ports/ARM-Cortex-M/ARMv6-M-RVM "
GUESTS+="uCOSII_V293/Ports/ARM-Cortex-M/ARMv7-M-RVM "
GUESTS+="uCOSII_V293/Ports/RISC-V/RV32-RVM "
GUESTS+="uCOSII_V293/Test "

# RTEMS 7.0 RTE guest
# Directory-level entries keep the list compact.  Build-time staging files
# (start/rte_entry*, include/rte_prc.h, include/rte_config.h and app/) and
# sample build artifacts are removed from the destination after copying.
GUESTS+="RTEMS_V701/bsps/arm/v7m_rvm "
GUESTS+="RTEMS_V701/spec/build/bsps/arm/v7m_rvm "
GUESTS+="RTEMS_V701/spec/build/testsuites/rte "
GUESTS+="RTEMS_V701/spec/build/testsuites/grp.yml "
GUESTS+="RTEMS_V701/spec/build/cpukit/cpuarm.yml "
GUESTS+="RTEMS_V701/spec/build/cpukit/cpuarm_rvm.yml "
GUESTS+="RTEMS_V701/spec/build/cpukit/librtemscpu.yml "
GUESTS+="RTEMS_V701/spec/build/cpukit/optincludes.yml "
GUESTS+="RTEMS_V701/spec/build/cpukit/optlibdebugger.yml "
GUESTS+="RTEMS_V701/testsuites/samples/rte_hello "
GUESTS+="RTEMS_V701/testsuites/samples/rte_tick "
GUESTS+="RTEMS_V701/testsuites/samples/rte_two_task_fpu "
GUESTS+="RTEMS_V701/testsuites/samples/rte_benchmark "
# The RVM CPU port is a new port; the upstream ARM sources are untouched.
GUESTS+="RTEMS_V701/cpukit/score/cpu/arm/v7m_rvm "
GUESTS+="RTEMS_V701/wscript "

# NuttX 13.1.0 NTX guest.  The port is a new chip directory plus a new board
# (the board also carries the benchmark and its chip headers under test/);
# the shared arch files carry the small CONFIG_ARCH_CHIP_ARMV7M_RVM hooks.
GUESTS+="NuttX_V131/nuttx/arch/arm/src/armv7-m-rvm "
GUESTS+="NuttX_V131/nuttx/arch/arm/include/armv7-m-rvm "
GUESTS+="NuttX_V131/nuttx/boards/arm/armv7-m-rvm "
GUESTS+="NuttX_V131/nuttx/arch/arm/Kconfig "
GUESTS+="NuttX_V131/nuttx/arch/arm/include/irq.h "
GUESTS+="NuttX_V131/nuttx/arch/arm/include/armv7-m/irq.h "
GUESTS+="NuttX_V131/nuttx/arch/arm/src/CMakeLists.txt "
GUESTS+="NuttX_V131/nuttx/boards/Kconfig "

# Zephyr 3.7.0 LTS ZPR guest.  Only the port, the benchmark app, the virtual
# ARMv7-M SoC/board and the arch files changed for CONFIG_RVM_GUEST are
# mirrored; the rest of the west workspace is the vendor tree.
GUESTS+="Zephyr_V370/zephyr/boards/rvm/armv7m "
GUESTS+="Zephyr_V370/zephyr/arch/arm/core/CMakeLists.txt "
GUESTS+="Zephyr_V370/zephyr/arch/arm/core/cortex_m/Kconfig "
GUESTS+="Zephyr_V370/zephyr/arch/arm/core/cortex_m/CMakeLists.txt "
GUESTS+="Zephyr_V370/zephyr/arch/arm/core/cortex_m/v7m_rvm "
GUESTS+="Zephyr_V370/zephyr/arch/arm/include/kernel_arch_func.h "
GUESTS+="Zephyr_V370/zephyr/soc/arm/soc.yml "
GUESTS+="Zephyr_V370/zephyr/soc/arm/v7m_rvm "
GUESTS+="Zephyr_V370/zephyr/dts/bindings/vendor-prefixes.txt "

rm -rf ./*/

for GUEST in $GUESTS; do
    SRC="${SRC_PREFIX}${GUEST}"
    if [ -d "$SRC" ]; then
        echo "==> $GUEST"
        mkdir -p "${DST_PREFIX}${GUEST}"
        cp -r "${SRC}/"* "${DST_PREFIX}${GUEST}/"
    elif [ -f "$SRC" ]; then
        echo "==> $GUEST"
        mkdir -p "$(dirname "${DST_PREFIX}${GUEST}")"
        cp "${SRC}" "${DST_PREFIX}${GUEST}"
    else
        echo "--> skip (not yet present): $GUEST"
    fi
done

# RTE build-time staging and sample artifacts are generated, not distributed.
RTE_BSP="RTEMS_V701/bsps/arm/v7m_rvm"
rm -f "$RTE_BSP/start/rte_entry.c" "$RTE_BSP/start/rte_entry_desc.c" \
      "$RTE_BSP/start/rvm_platform_a7m_gcc.S" \
      "$RTE_BSP/include/rte_prc.h" "$RTE_BSP/include/rte_config.h" \
      "$RTE_BSP/include/rvm_platform.h" \
      "$RTE_BSP/include/rvm_platform_a7m_conf.h" \
      "$RTE_BSP"/include/prc_*.h
rm -rf "$RTE_BSP/app"
find "RTEMS_V701/testsuites/samples" -type f      \( -name '*.o' -o -name '*.bin' -o -name '*.exe' \) -delete 2>/dev/null || true

echo "done"
