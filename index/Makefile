TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=/home/nikita/work/ri/extern/BOOST/lib
BOOST_INCLUDE=/home/nikita/work/ri/extern/BOOST/include

all: indexer

indexer: reader.o input_handler.o sorter.o
	g++-8 -std=c++17 -O2 reader.o input_handler.o sorter.o make_index.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindex_maker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

reader.o: reader.cpp
	g++-8 -std=c++17 -O2 -c reader.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindex_maker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

input_handler.o: input_handler.cpp
	g++-8 -std=c++17 -O2 -c input_handler.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindex_maker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

sorter.o: sorter.cpp
	g++-8 -std=c++17 -O2 -c sorter.cpp

output_handler.o: output_handler.cpp
	g++-8 -std=c++17 -O2 -c output_handler.cpp

clean:
	rm -f *.o
clean_all:
	rm -f *.o *.out
