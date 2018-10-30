#!/bin/bash

# Compiling repository
mkdir build/
cd build/
cmake -G "Unix Makefiles" -Dbuild_parse=ON
make

cd ..
rm -rf build/
