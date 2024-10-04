#!/usr/bin/bash
# pip install meson ninja
# export LIBRARY_PATH=~/.local/lib
# export CPATH=~/.local/include
# export CC=gcc
type=debug
if [ ! -z "$1" ]; then type=$1; fi
meson setup --wipe --buildtype $type build --prefix ~/.local
cd build
ninja
