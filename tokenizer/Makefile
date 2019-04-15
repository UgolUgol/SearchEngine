TOOLS_LIB=../lib
TOOLS_INCLUDE=../include
BOOST_LIB=/home/nikita/work/ri/extern/BOOST/lib
BOOST_INCLUDE=/home/nikita/work/ri/extern/BOOST/include

all: tokenizer

tokenizer:
	g++-8 -std=c++17 tokenizer.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) -pthread
