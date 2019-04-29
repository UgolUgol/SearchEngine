TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=${BOOST_PATH}/lib
BOOST_INCLUDE=${BOOST_PATH}/include

all: tokenizer

tokenizer:
	g++ -std=c++17 tokenizer.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) -pthread
