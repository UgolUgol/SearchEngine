TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=${BOOST_PATH}/lib
BOOST_INCLUDE=${BOOST_PATH}/include

all: searcher

searcher: index expression_node
	g++ -std=c++17 -O2 -I $(TOOLS_INCLUDE) -I $(BOOST_INCLUDE) $(TOOLS_INCLUDE)/object_files/index.o \
	$(TOOLS_INCLUDE)/object_files/expression_node.o make_search.cpp -o searcher.out

expression_node: $(TOOLS_INCLUDE)/expression_node.cpp
	g++ -std=c++17 -I $(BOOST_INCLUDE) -c $(TOOLS_INCLUDE)/expression_node.cpp -o $(TOOLS_INCLUDE)/object_files/expression_node.o

index: $(TOOLS_INCLUDE)/index.cpp
	g++ -std=c++17 -I $(BOOST_INCLUDE) -c $(TOOLS_INCLUDE)/index.cpp -o $(TOOLS_INCLUDE)/object_files/index.o


clean:
	rm -f $(TOOLS_INCLUDE)/object_files/index.o $(TOOLS_INCLUDE)/object_files/expression_node.o
clean_all:
	rm -f $(TOOLS_INCLUDE)/object_files/index.o $(TOOLS_INCLUDE)/object_files/expression_node.o *.out

