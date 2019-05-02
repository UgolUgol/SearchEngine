#pragma once 

#include <vector>
#include <string>
#include <iostream>
#include "index_iterator.h"


struct DefaultIndex {};


template<typename IndexType> struct IndexTraits { };
template<> 
struct IndexTraits<DefaultIndex> { 

	struct Dictionary {
		using Type = size_t;
		static constexpr size_t NodeSize = 3 * sizeof(size_t);

		struct CoordOffset { 
			using Type = size_t;
			static constexpr size_t Offset =  1 * sizeof(size_t);
		};
		struct Length { 
			using Type = size_t;
			static constexpr size_t Offset = 2 * sizeof(size_t);
		};
	};


	struct CoordinateFile { 
		using Type = size_t;
		static constexpr size_t NodeSize = 2 * sizeof(size_t);

		struct Position {
			using Type = size_t;
			static constexpr size_t Offset = 1 * sizeof(size_t);
		};
	};
};



template<typename IndexType = DefaultIndex>
class Index {
public:
	using DictionaryIterator = IndexIterator<typename IndexTraits<IndexType>::Dictionary::Type, 
											 IndexTraits<IndexType>::Dictionary::NodeSize>;
											 
	using CoordinateBlocksIterator = IndexIterator<typename IndexTraits<IndexType>::CoordinateFile::Type,
												   IndexTraits<IndexType>::CoordinateFile::NodeSize>;

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
		auto raw_ptr = reinterpret_cast<RawMemory>(mappedDict.get_address()) + mappedDict.get_size();
		return DictionaryIterator(static_cast<void*>(raw_ptr));
	}

	auto coordBegin() {
		return CoordinateBlocksIterator(mappedDict.get_address());
	}
	auto coordEnd() {
		auto raw_ptr = reinterpret_cast<RawMemory>(mappedCoord.get_address()) + mappedCoord.get_size();
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

