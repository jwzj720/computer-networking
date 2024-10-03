#!/bin/bash

# Define the device and mount point
DEVICE="/dev/sda1"
MOUNT_POINT="/mnt"
PI_REPO=~/computer-networking
THUMB_DRIVE=$MOUNT_POINT/computer-networking.git

# Mount the USB drive
sudo mount -o uid=1001,gid=1001 $DEVICE $MOUNT_POINT

# Check if the mount was successful
if [ $? -eq 0 ]; then
    echo "Drive mounted successfully at $MOUNT_POINT."

    # Navigate to the project folder
    cd $PI_REPO

    # Ensure you're on the correct branch
    git checkout main

    # Pull changes from the thumb drive repository
    git pull $THUMB_DRIVE main

    echo "Changes pulled from the thumb drive."

    # Optionally, unmount the drive after pulling changes
    sudo umount $MOUNT_POINT
    echo "Drive unmounted."
else
    echo "Failed to mount the drive."
fi
