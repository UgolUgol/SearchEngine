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

	struct DirectIndex {

		struct DocId {
			using Type = size_t;
			static constexpr size_t offset = 1;
		};
		struct Offset {
			using Type = size_t;
			static constexpr size_t offset = 2;
		};
		struct NameLength {
			using Type = size_t;
			static constexpr size_t offset = 3;
		};
		struct UrlLength {
			using Type = size_t;
			static constexpr size_t offset = 4;
		};

		static constexpr size_t NodeSize = 4 * sizeof(size_t);
	};
};



template<typename IndexType = DefaultIndex>
class Index {
public:

	using DictionaryNodeType = typename IndexTraits<IndexType>::Dictionary::Type;
	using CoordinateFileNodeType = typename IndexTraits<IndexType>::Dictionary::Type;
	using DictionaryOffsetNodeType = typename IndexTraits<IndexType>::Dictionary::CoordOffset::Type;
	using DictionaryLengthNodeType = typename IndexTraits<IndexType>::Dictionary::Length::Type;
	using CoordinateFilePositionNodeType = typename IndexTraits<IndexType>::CoordinateFile::Position::Type;

	using DictionaryIterator = IndexIterator<DictionaryNodeType, IndexTraits<IndexType>::Dictionary::NodeSize>;
	using CoordinateBlocksIterator = IndexIterator<CoordinateFileNodeType, IndexTraits<IndexType>::CoordinateFile::NodeSize>;

	Index() = delete;
	Index(const Index&) = delete;
	Index(Index&&) = default;
	Index(const char*, const char*);

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
		return CoordinateBlocksIterator(mappedCoord.get_address());
	}
	auto coordEnd() {
		auto raw_ptr = reinterpret_cast<RawMemory>(mappedCoord.get_address()) + mappedCoord.get_size();
		return CoordinateBlocksIterator(static_cast<void*>(raw_ptr));
	}

	DictionaryOffsetNodeType getOffset(DictionaryIterator iterator);
	DictionaryLengthNodeType getLength(DictionaryIterator iterator);

private:
	boost::interprocess::file_mapping dict, coord;
	boost::interprocess::mapped_region mappedDict, mappedCoord;
};


template<typename IndexType=DefaultIndex>
class DirectIndex {
public:
	using DirectIndexTraits = typename IndexTraits<IndexType>::DirectIndex;
	using DocId = typename DirectIndexTraits::DocId;
	using Offset = typename DirectIndexTraits::Offset;
	using NameLength = typename DirectIndexTraits::NameLength;
	using UrlLength = typename DirectIndexTraits::UrlLength;

	using DocIdIterator = IndexIterator<typename DocId::Type, DirectIndexTraits::NodeSize>;
	using OffsetIterator = IndexIterator<typename Offset::Type, DirectIndexTraits::NodeSize>;
	using NameLengthIterator = IndexIterator<typename NameLength::Type, DirectIndexTraits::NodeSize>;
	using UrlLengthIterator = IndexIterator<typename UrlLength::Type, DirectIndexTraits::NodeSize>;

	DirectIndex() = delete;
	DirectIndex(const DirectIndex&) = delete;
	DirectIndex(DirectIndex&&) = default;
	DirectIndex(const char*);

	DirectIndex& operator=(const DirectIndex&) = delete;
	DirectIndex& operator=(DirectIndex&&) = default;

	template<typename IteratorType>
	auto begin() {

		auto begin = static_cast<size_t*>(mappedInvCoord.get_address()) + IteratorType::offset;
		return IndexIterator<typename IteratorType::Type, DirectIndexTraits::NodeSize>(static_cast<void*>(begin));

	}

	template<typename IteratorType>
	auto end() {

		auto endPosition = *static_cast<size_t*>(mappedInvCoord.get_address()) + sizeof(size_t)*IteratorType::offset;
		auto end = static_cast<RawMemory>(mappedInvCoord.get_address()) + endPosition;

		return IndexIterator<typename IteratorType::Type, DirectIndexTraits::NodeSize>(static_cast<void*>(end));

	}

private:
	boost::interprocess::file_mapping invCoord;
	boost::interprocess::mapped_region mappedInvCoord;
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

