#!/bin/bash

rm -Rf build
mkdir build
cd build

cmake_flags=${cmake_flags}' -DBUILD_TESTING=1 -DBUILD_CLEAN=1'

cmake $cmake_flags ../ && make clean && make

cd ..
cp build/tests/crdt_test tests_prebuilt/crdt_test
