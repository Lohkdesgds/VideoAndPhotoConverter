#!/bin/bash

rm -rf build
rm -rf vcpkg_installed

./vcpkg/vcpkg install

ARGUMENT=-DCMAKE_TOOLCHAIN_FILE="$PWD"/vcpkg/scripts/buildsystems/vcpkg.cmake
export OUTPUT="$PWD"/build/output

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$OUTPUT -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$OUTPUT -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$OUTPUT $ARGUMENT -S . -B build 2>&1 | tee runner.txt
cmake --build build 2>&1 | tee runner.txt -a;

#echo "Searching for build executables (matches)..."
#
#cat runner.txt | grep Built | awk '{print $NF}' > raw_execs.txt
#
#find "build" -perm -111 -type f | grep -v -E ".*[\.out]$|.*[\.bin]$" | grep $(<raw_execs.txt) > execs.txt
#
#cat execs.txt
find $OUTPUT -type f

echo "DONE!"
