#!/usr/bin/bash
# pip install meson ninja
# export CC=gcc
type=debug
if [ ! -z "$1" ]; then type=$1; fi
meson setup --wipe --buildtype $type build
cd build
ninja
