TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=${BOOST_PATH}/lib
BOOST_INCLUDE=${BOOST_PATH}/include

all: searcher

searcher: index
	g++ -std=c++17 -I $(TOOLS_INCLUDE) -I $(BOOST_INCLUDE) $(TOOLS_INCLUDE)/object_files/index.o make_search.cpp

index: $(TOOLS_INCLUDE)/index.cpp
	g++ -std=c++17 -I $(BOOST_INCLUDE) -c $(TOOLS_INCLUDE)/index.cpp -o $(TOOLS_INCLUDE)/object_files/index.o


clean:
	rm -f $(TOOLS_INCLUDE)/object_files/index.o $(TOOLS_INCLUDE)/object_files/expression_node.o
clean_all:
	rm -f $(TOOLS_INCLUDE)/object_files/index.o $(TOOLS_INCLUDE)/object_files/expression_node.o *.out

