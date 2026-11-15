#!/bin/sh

# See if we're really executing this inside the "Project" folder
if [ $(basename "$PWD") != "Project" ]; then
   echo "Must be run in the Project folder!"
   exit
fi

# Dangerous operation: removing every subfolder - including symbolic links!
# The kept workspaces below are exempt - they hold the shipping example
# (also whitelisted in .gitignore).
KEEP="GCCMF-STM32H743ZIT6-AQUARIUM"
for dir in ./*-*/; do
    [ "$dir" = "./$KEEP/" ] && continue
    rm -rf "$dir"
done
rm -f ./*.log