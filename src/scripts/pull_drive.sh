#!/bin/bash

# Path to local project on Pi
PI_REPO=~/computer-networking
THUMB_DRIVE=/Volumes/BITTIES/computer-networking.git

# Pull changes from thumb drive
cd $PI_REPO

# Ensure you're on the correct branch (main in this example)
git checkout main

# Fetch changes from the thumb drive and merge them
git pull $THUMB_DRIVE main

# Output success message
echo "Changes pulled from thumb drive and merged into local repository."
