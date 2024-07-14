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
RVGEN="../Utility/M7M02_Projgen/Release/rvgen.exe"
KERNEL="../../M7M01_Eukaron/"
MONITOR="../"
RMP="../../M5P01_Prokaron/"
FRT="../../M0A00_Library/"

# Process project paths
PROJFULL=$(basename $1)
PROJNAME="${PROJFULL%.*}"
PROJUPPER=$(echo $PROJNAME | tr '[:lower:]' '[:upper:]')
WORKSPACE=./$PROJUPPER/

# Do the project generation
COMMAND="$RVGEN -p $1 -k $KERNEL -m $MONITOR -rmp $RMP -frt $FRT -w $WORKSPACE -v"
echo $COMMAND
$COMMAND > ./log.txt


