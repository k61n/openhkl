#!/bin/bash
if [ $# -ne 4 ]; then
    echo "Invalid number of arguments"
    exit 1
fi

USERNAME=$1
USERID=$2
GROUPNAME=$3
GROUPID=$4

# Create the correct user and group to match the host's, so that
# the X connection works properly.
groupadd -g $GROUPID $GROUPNAME
useradd -m -g $GROUPID -u $USERID $USERNAME

su $USERNAME
/openhkl/build/main/OpenHKL
