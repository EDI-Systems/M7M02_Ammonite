#!/bin/sh

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

rm -rf ./*/

for GUEST in $GUESTS; do
    if [ ! -d "${SRC_PREFIX}${GUEST}" ]; then
        echo "--> skip (not yet present): $GUEST"
        continue
    fi
    echo "==> $GUEST"
    mkdir -p "${DST_PREFIX}${GUEST}"
    cp -r "${SRC_PREFIX}${GUEST}/"* "${DST_PREFIX}${GUEST}/"
done

echo "done"
