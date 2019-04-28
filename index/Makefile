TOOLS_LIB=../lib
TOOLS_INCLUDE=../include

all: indexer

indexer: 
	g++-8 -std=c++17 -O2 make_index.cpp -L $(TOOLS_LIB) -ltools -lindex_maker -Wl,-rpath=$(TOOLS_LIB) -I $(TOOLS_INCLUDE) 

clean:
	rm -f *.o
clean_all:
	rm -f *.o *.out
