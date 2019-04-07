TOOLS_LIB=/mnt/d/search/lib
TOOLS_INCLUDE=/mnt/d/search/include
BOOST_LIB=/mnt/d/Загрузки/boost_1_55_0/build/lib
BOOST_INCLUDE=/mnt/d/Загрузки/boost_1_55_0/build/include


all: indexer

indexer: reader.o input_handler.o sorter.o
	g++-8 -std=c++17 reader.o input_handler.o sorter.o make_index.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindmaker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

reader.o: reader.cpp
	g++-8 -std=c++17 -c reader.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindmaker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

input_handler.o: input_handler.cpp
	g++-8 -std=c++17 -c input_handler.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -lindmaker -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) 

sorter.o: sorter.cpp
	g++-8 -std=c++17 -c sorter.cpp

clean:
	rm -f *.o
clean_all:
	rm -f *.o *.out