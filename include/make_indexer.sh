#!/bin/sh

BOOST='/home/nikita/work/ri/extern/BOOST'
TOOLS_INCLUDE='.'
LIBS='../lib'

rm -rf object_files
mkdir object_files
g++ -std=c++17 -c -Wall -Werror -O2 -fpic reader.cpp -I $TOOLS_INCLUDE -o object_files/reader.o
g++ -std=c++17 -c -Wall -Werror -O2 -fpic input_handler.cpp -I $TOOLS_INCLUDE -o object_files/input_handler.o
g++ -std=c++17 -c -Wall -Werror -O2 -fpic sorter.cpp -I $TOOLS_INCLUDE -o object_files/sorter.o
g++ -std=c++17 -c -Wall -Werror -O2 -fpic output_handler.cpp -I $TOOLS_INCLUDE -o object_files/output_handler.o
g++ -std=c++17 -c -Wall -Werror -O2 -fpic writer.cpp  -I $TOOLS_INCLUDE -o object_files/writer.o
g++ -std=c++17 -c -Wall -Werror -O2 -fpic index_maker.cpp -o object_files/index_maker.o
g++ -shared -o $LIBS/libindex_maker.so object_files/*.o -L $LIBS -ltools -Wl,-rpath=$TOOLS

