#!/bin/sh

# See if we're really executing this inside the "Project" folder
if [ $(basename "$PWD") != "Project" ]; then
   echo "Must be run in the Project folder!"
   exit
fi

# Dangerous operation: removing every subfolder - including symbolic links!
rm -rf ./*-*/
rm -f ./log.txt