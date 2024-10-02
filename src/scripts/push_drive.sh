#!/bin/bash

# Path to local project
LOCAL_REPO=~/CC/Networking/computer-networking
THUMB_DRIVE=/Volumes/BITTIES/computer-networking.git

# Commit and push to GitHub
cd $LOCAL_REPO
git add .
git commit -m "Syncing changes to GitHub"
git push origin main
echo "Changes pushed to GitHub."

# Push to thumb drive for Pi
git push pi main
echo "Changes pushed to thumb drive."
