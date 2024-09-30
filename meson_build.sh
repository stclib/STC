#!/usr/bin/bash
# pip install meson ninja
# export CC=gcc
meson setup --wipe --buildtype debug build
cd build
ninja
