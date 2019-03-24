TOOLS_LIB=/mnt/d/search/lib
TOOLS_INCLUDE=/mnt/d/search/include
BOOST_LIB=/mnt/d/Загрузки/boost_1_55_0/build/lib
BOOST_INCLUDE=/mnt/d/Загрузки/boost_1_55_0/build/include


all: tokenizer

tokenizer:
	g++-8 -std=c++17 tokenizer.cpp -L $(BOOST_LIB) -lboost_regex -Wl,-rpath=$(BOOST_LIB) \
	-L $(TOOLS_LIB) -ltools -Wl,-rpath=$(TOOLS_LIB) -I $(BOOST_INCLUDE) -I $(TOOLS_INCLUDE) -pthread