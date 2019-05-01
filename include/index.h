#pragma once 

#include <vector>
#include <string>
#include <iostream>
#include "index_iterator.h"


struct DefaultIndex {
	static constexpr size_t DictNodeSize = 3;
	static constexpr size_t CoordNodeSize = 2;
	using NodeType = size_t;
};


template<typename NodeType=typename DefaultIndex::NodeType,
		 size_t DictNodeSize=DefaultIndex::DictNodeSize,
 		 size_t CoordNodeSize=DefaultIndex::CoordNodeSize>
class Index {
public:
	using DictionaryIterator = IndexIterator<NodeType, DictNodeSize>;
	using CoordinateBlocksIterator = IndexIterator<NodeType, CoordNodeSize>;

	Index() = delete;
	Index(const Index&) = delete;
	Index(Index&&) = default;
	Index(const char*, const char*, const char*);

	Index& operator=(const Index&) = delete;
	Index& operator=(Index&&) = default;
	DictionaryIterator dictionaryBegin();
	DictionaryIterator dictionaryEnd();

	CoordinateBlocksIterator coordBegin();
	CoordinateBlocksIterator coordEnd();

private:
	boost::interprocess::file_mapping dict, coord, invCoord;
	boost::interprocess::mapped_region mappedDict, mappedCoord, mappedInvCoord;
};



