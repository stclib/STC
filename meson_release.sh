#!/usr/bin/bash
export CC=gcc
meson setup --wipe --buildtype release release
cd release
ninja
