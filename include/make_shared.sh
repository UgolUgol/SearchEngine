#!/bin/sh

filename=$1
g++ -std=c++17 -c -Wall -Werror -O2 -fpic $filename.cpp
g++ -shared -o lib$filename.so $filename.o
mv lib$filename.so ../lib/
rm -rf $filename.o
