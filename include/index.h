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

	auto dictionaryBegin() {
		return DictionaryIterator(mappedDict.get_address());
	}
	auto dictionaryEnd() {
		auto raw_ptr = reinterpret_cast<char*>(mappedDict.get_address()) + mappedDict.get_size();
		return DictionaryIterator(static_cast<void*>(raw_ptr));
	}

	auto coordBegin() {
		return CoordinateBlocksIterator(mappedDict.get_address());
	}
	auto coordEnd() {
		auto raw_ptr = reinterpret_cast<char*>(mappedCoord.get_address()) + mappedCoord.get_size();
		return CoordinateBlocksIterator(static_cast<void*>(raw_ptr));
	}

private:
	boost::interprocess::file_mapping dict, coord, invCoord;
	boost::interprocess::mapped_region mappedDict, mappedCoord, mappedInvCoord;
};

namespace algorithms {

		template<typename InputIterator> 
		InputIterator findHash(InputIterator begin, InputIterator end, const typename InputIterator::value_type& hash) {

			auto range = std::equal_range(begin, end, hash);
			if(range.first == range.second) {
				return end;
			}
			return range.first;
		}


}

