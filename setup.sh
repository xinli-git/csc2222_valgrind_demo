#!/bin/bash


if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root, but don't worry, it will just apt-install a package" 
   exit 1
fi
apt-get update
apt-get install png++
