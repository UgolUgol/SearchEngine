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


	struct DictionaryTest {

		struct Hash {
			using Type = size_t;
			static constexpr size_t Offset = 0;
			static constexpr size_t NodeSize = 3 * sizeof(size_t);
		};

		struct OffsetInfo {
			using Type = size_t;
			static constexpr size_t Offset = 1;
			static constexpr size_t NodeSize = 3 * sizeof(size_t);
		};

		struct LengthInfo {
			using Type = size_t;
			static constexpr size_t Offset = 2;
			static constexpr size_t NodeSize = 3 * sizeof(size_t);
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
			static constexpr size_t NodeSize = 4 * sizeof(size_t);
		};
		struct Offset {
			using Type = size_t;
			static constexpr size_t offset = 2;
			static constexpr size_t NodeSize = 4 * sizeof(size_t);
		};
		struct NameLength {
			using Type = size_t;
			static constexpr size_t offset = 3;
			static constexpr size_t NodeSize = 4 * sizeof(size_t);
		};
		struct UrlLength {
			using Type = size_t;
			static constexpr size_t offset = 4;
			static constexpr size_t NodeSize = 4 * sizeof(size_t);
		};

		struct Raw {
			using Type = char;
			static constexpr size_t offset = 0;
			static constexpr size_t NodeSize = sizeof(Type);
		};

		
	};
};


struct SearchResultBlock {
	std::wstring name;
	std::wstring url;
};

template<typename IndexType = DefaultIndex>
class Index {
public:

/*	using DictionaryNodeType = typename IndexTraits<IndexType>::Dictionary::Type;
	using DictionaryOffsetNodeType = typename IndexTraits<IndexType>::Dictionary::CoordOffset::Type;
	using DictionaryLengthNodeType = typename IndexTraits<IndexType>::Dictionary::Length::Type;
	using CoordinateFilePositionNodeType = typename IndexTraits<IndexType>::CoordinateFile::Position::Type;
	using DictionaryIterator = IndexIterator<DictionaryNodeType, IndexTraits<IndexType>::Dictionary::NodeSize>;*/

	using DictionaryTraits = typename IndexTraits<IndexType>::DictionaryTest;
	using Hash = typename DictionaryTraits::Hash;
	using OffsetInfo = typename DictionaryTraits::OffsetInfo;
	using LengthInfo = typename DictionaryTraits::LengthInfo;

	using CoordinateFileNodeType = typename IndexTraits<IndexType>::Dictionary::Type;

	using HashIterator = IndexIterator<typename Hash::Type, Hash::NodeSize>;
	using OffsetIterator = IndexIterator<typename OffsetInfo::Type, OffsetInfo::NodeSize>;
	using LengthIterator = IndexIterator<typename LengthInfo::Type, LengthInfo::NodeSize>;
	using CoordinateBlocksIterator = IndexIterator<CoordinateFileNodeType, IndexTraits<IndexType>::CoordinateFile::NodeSize>;

	Index() = delete;
	Index(const Index&) = delete;
	Index(Index&&) = default;
	Index(const char*, const char*);

	Index& operator=(const Index&) = delete;
	Index& operator=(Index&&) = default;

	template<typename IteratorType> auto dictionaryBegin() const {

		auto begin = static_cast<size_t*>(mappedDict.get_address()) + IteratorType::Offset;
		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(begin));

	}

	template<typename IteratorType> auto dictionaryEnd() const {

		auto offset = sizeof(typename IteratorType::Type) * IteratorType::Offset;
		auto end = reinterpret_cast<RawMemory>(mappedDict.get_address()) + offset + mappedDict.get_size();

		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(end));
	
	}

	auto coordBegin() const {
		return CoordinateBlocksIterator(mappedCoord.get_address());
	}
	auto coordEnd() const {
		auto raw_ptr = reinterpret_cast<RawMemory>(mappedCoord.get_address()) + mappedCoord.get_size();
		return CoordinateBlocksIterator(static_cast<void*>(raw_ptr));
	}

	typename OffsetInfo::Type getOffset(HashIterator iterator) const;
	typename LengthInfo::Type getLength(HashIterator iterator) const;

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
	using Raw = typename DirectIndexTraits::Raw;

	using DocIdIterator = IndexIterator<typename DocId::Type, DocId::NodeSize>;
	using OffsetIterator = IndexIterator<typename Offset::Type, Offset::NodeSize>;
	using NameLengthIterator = IndexIterator<typename NameLength::Type, NameLength::NodeSize>;
	using UrlLengthIterator = IndexIterator<typename UrlLength::Type, UrlLength::NodeSize>;
	using RawIterator = IndexIterator<typename Raw::Type, Raw::NodeSize>;

	DirectIndex() = delete;
	DirectIndex(const DirectIndex&) = delete;
	DirectIndex(DirectIndex&&) = default;
	DirectIndex(const char*);

	DirectIndex& operator=(const DirectIndex&) = delete;
	DirectIndex& operator=(DirectIndex&&) = default;

	template<typename IteratorType>
	auto begin() const {

		auto begin = static_cast<size_t*>(mappedInvCoord.get_address()) + IteratorType::offset;
		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(begin));

	}

	template<typename IteratorType>
	auto end() const {

		auto endPosition = *static_cast<size_t*>(mappedInvCoord.get_address()) + sizeof(size_t)*IteratorType::offset;
		auto end = static_cast<RawMemory>(mappedInvCoord.get_address()) + endPosition;

		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(end));

	}

private:
	boost::interprocess::file_mapping invCoord;
	boost::interprocess::mapped_region mappedInvCoord;
};


namespace algorithms {

		template<typename InputIterator> 
		InputIterator findInIndex(InputIterator begin, InputIterator end, const typename InputIterator::value_type& value) {

			auto range = std::equal_range(begin, end, value);
			if(range.first == range.second) {
				return end;
			}

			return range.first;
		}

		template<typename Container, typename IndexType>
		std::vector<SearchResultBlock> formSearchResult(const Container& docIds, const DirectIndex<IndexType>& index) {

			using DIndex = DirectIndex<IndexType>;
			std::vector<SearchResultBlock> results;
			results.reserve(docIds.size());

			for(const auto& docId : docIds) {

				auto iterator = findInIndex(index.template begin<typename DIndex::DocId>(),
				 							index.template end<typename DIndex::DocId>(), docId);
				auto position = std::distance(index.template begin<typename DIndex::DocId>(), iterator);

				auto offset = *(index.template begin<typename DIndex::Offset>() + position);
				auto nameLength = *(index.template begin<typename DIndex::NameLength>() + position);
				auto urlLength = *(index.template begin<typename DIndex::UrlLength>() + position);

				auto articleInfoBegin = index.template begin<typename DIndex::Raw>() + offset;
				auto rawArticleInfoBegin = reinterpret_cast<wchar_t*>(articleInfoBegin.rawPointer());

				std::wstring name(rawArticleInfoBegin, rawArticleInfoBegin + nameLength);
				std::wstring url(rawArticleInfoBegin + nameLength, rawArticleInfoBegin + nameLength + urlLength);

				results.push_back(SearchResultBlock{name, url});
			}

			return results;
		} 

}

