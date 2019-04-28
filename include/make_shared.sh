#!/bin/sh

filename=$1
g++-8 -std=c++17 -c -Wall -Werror -O2 -fpic $filename.cpp
g++-8 -shared -o lib$filename.so $filename.o
mv lib$filename.so ../lib/
rm -rf $filename.o
