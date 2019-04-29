TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=${BOOST_PATH}/lib
BOOST_INCLUDE=${BOOST_PATH}/include

all: searcher

searcher:
	g++-8 -std=c++17 -I $(TOOLS_INCLUDE) -I $(BOOST_INCLUDE) make_search.cpp -lstdc++fs -o search 
