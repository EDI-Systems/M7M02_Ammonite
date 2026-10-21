#!/bin/sh

SRC_PREFIX="../../"
DST_PREFIX="./"

GUESTS="FreeRTOS_V900/portable/GCC/A6M_RVM "
GUESTS+="FreeRTOS_V900/portable/GCC/A7M_RVM "
GUESTS+="FreeRTOS_V900/portable/GCC/RV32P_RVM "
GUESTS+="FreeRTOS_V900/portable/Keil/A6M_RVM "
GUESTS+="FreeRTOS_V900/portable/Keil/A7M_RVM "
GUESTS+="FreeRTOS_V900/portable/Keil/RV32P_RVM "
GUESTS+="FreeRTOS_V900/test "

rm -rf ./*/

for GUEST in $GUESTS; do
    echo "==> $GUEST"
    mkdir -p "${DST_PREFIX}${GUEST}"
    cp -r "${SRC_PREFIX}${GUEST}/"* "${DST_PREFIX}${GUEST}/"
done

echo "done"