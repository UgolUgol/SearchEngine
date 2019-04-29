#pragma once 

#include <vector>
#include <string>
#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>


struct DefaultIndex {
	static constexpr size_t DictNodeSize = sizeof(size_t) * 3;
	static constexpr size_t CoordNodeSize = sizeof(size_t) * 2;
};


template<size_t DictNodeSize=DefaultIndex::DictNodeSize,
 		 size_t CoordNodeSize=DefaultIndex::CoordNodeSize>
class Index {
public:
/*	using DictionaryIterator = IndexIterator<DictNodeSize>;
	using CoordinateBlocksIterator = IndexIterator<CoordNodeSize>;*/

	Index() = delete;
	Index(const Index&) = delete;
	Index(Index&&) = default;
	Index(const char*, const char*, const char*);

	Index& operator=(const Index&) = delete;
	Index& operator=(Index&&) = default;
private:
	boost::interprocess::file_mapping dict, coord, invCoord;
	boost::interprocess::mapped_region mappedDict, mappedCoord, mappedInvCoord;
};



