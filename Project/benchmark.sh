#!/bin/sh

# See if we're really executing this inside the "Project" folder
if [ $(basename "$PWD") != "Project" ]; then
   echo "Must be run in the Project folder!"
   exit
fi

# See if the first parameter is present
if [ "$#" -ne 1 ]; then
   echo "Usage: ./generate.sh project.rvp"
   exit
fi

# Supply default paths
RVGEN="../Utility/M7M02_Final/rvgen.exe"
KERNEL="../../M7M01_Eukaron/"
MONITOR="../"
RMP="../../M5P01_Prokaron/"
FRT="../../FreeRTOS_V900/"
UO3="../../uCOSIII_V308/"
RTT="../../RTThread_V411/"
DJY=../../DjyOS_V110/
KLT=../../KLite_V521/
UO2=../../uCOSII_V293/
RTE="../../RTEMS_V701/"
RTEMP="C:/Toolchain/RTEMS7"
RTE_APP="rte_benchmark"

# Process project paths
PROJFULL=$(basename $1)
PROJNAME="${PROJFULL%.*}"
PROJUPPER=$(echo $PROJNAME | tr '[:lower:]' '[:upper:]')
WORKSPACE=./$PROJUPPER/

# Do the project generation
COMMAND="$RVGEN -p $1 -k $KERNEL -m $MONITOR -rmp $RMP -frt $FRT -uo3 $UO3 -rtt $RTT -djy $DJY -klt $KLT -uo2 $UO2 -rte $RTE -rte-prefix $RTEMP -rte-app $RTE_APP -w $WORKSPACE -r report.log -v -b"
echo $COMMAND
$COMMAND > ./output.log


