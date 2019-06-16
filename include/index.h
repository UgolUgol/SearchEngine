#pragma once 

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include "index_iterator.h"


struct DefaultIndex {};


template<typename IndexType> struct IndexTraits { };
template<> 
struct IndexTraits<DefaultIndex> { 

	struct Dictionary {

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

		struct DocId {
			using Type = size_t;
			static constexpr size_t Offset = 0;
			static constexpr size_t NodeSize = 2 * sizeof(size_t);
		};

		struct Position {
			using Type = size_t;
			static constexpr size_t Offset = 1;
			static constexpr size_t NodeSize = 2 * sizeof(size_t);
		};
	};

	struct CompressedCoordinateFile {
		using Type = unsigned char;
		static constexpr size_t Offset = 0;
		static constexpr size_t NodeSize = sizeof(unsigned char); 
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


template<typename IndexType> 
class Index {
public:

	Index() = default;
	Index(const Index&) = delete;
	Index(Index&&) = default;

	Index& operator=(const Index&) = delete;
	Index& operator=(Index&&) = default;

	template<typename IteratorType> auto begin(void* address) const {

		auto begin = static_cast<size_t*>(address) + IteratorType::Offset;
		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(begin));

	}

	template<typename IteratorType> auto end(void* address, size_t size) const {

		auto offset = sizeof(typename IteratorType::Type) * IteratorType::Offset;
		auto end = reinterpret_cast<RawMemory>(address) + offset + size;

		return IndexIterator<typename IteratorType::Type, IteratorType::NodeSize>(static_cast<void*>(end));
	}
};


template<typename IndexType = DefaultIndex>
class DictionaryIndex : public Index<IndexType> {
public:
	using DictionaryTraits = typename IndexTraits<IndexType>::Dictionary;
	using Hash = typename DictionaryTraits::Hash;
	using OffsetInfo = typename DictionaryTraits::OffsetInfo;
	using LengthInfo = typename DictionaryTraits::LengthInfo;

	using HashIterator = IndexIterator<typename Hash::Type, Hash::NodeSize>;
	using OffsetIterator = IndexIterator<typename OffsetInfo::Type, OffsetInfo::NodeSize>;
	using LengthIterator = IndexIterator<typename LengthInfo::Type, LengthInfo::NodeSize>;

	DictionaryIndex(const char*);

	template<typename IteratorType> auto begin() const {

		return Index<IndexType>::template begin<IteratorType>(mappedDict.get_address());

	}

	template<typename IteratorType> auto end() const {

		return Index<IndexType>::template end<IteratorType>(mappedDict.get_address(), mappedDict.get_size());

	}

private:
	boost::interprocess::file_mapping dict;
	boost::interprocess::mapped_region mappedDict;

};

template<typename IndexType = DefaultIndex>
class CoordinateIndex : public Index<IndexType> {
public:
	using CoordinateFileTraits = typename IndexTraits<IndexType>::CoordinateFile;
	using CompressedCoordinateFile = typename IndexTraits<IndexType>::CompressedCoordinateFile;
	
	using DocId = typename CoordinateFileTraits::DocId;
	using Position = typename CoordinateFileTraits::Position;

	using DocIdIterator = IndexIterator<typename DocId::Type, DocId::NodeSize>;
	using PositionIterator = IndexIterator<typename Position::Type, Position::NodeSize>;
	using BytesIterator = IndexIterator<typename CompressedCoordinateFile::Type, CompressedCoordinateFile::NodeSize>;

	CoordinateIndex(const char*);

	template<typename IteratorType> auto begin() const {

		return Index<IndexType>::template begin<IteratorType>(mappedCoord.get_address());

	}

	template<typename IteratorType> auto end(void* address, size_t size) const {

		return Index<IndexType>::template end<IteratorType>(mappedCoord.get_address(), mappedCoord.get_size());

	}

private:
	boost::interprocess::file_mapping coord;
	boost::interprocess::mapped_region mappedCoord;

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


		template<typename From, typename To> IndexIterator<typename To::Type, To::NodeSize> 
		convertIterator(IndexIterator<typename From::Type, From::NodeSize> iterator)
		{	
			using OutputIterator = IndexIterator<typename To::Type, To::NodeSize>;

			constexpr auto sameTypes = std::is_same_v<typename From::Type, typename To::Type>;
			if constexpr (sameTypes) {
				
				auto address = iterator.rawPointer();
				bool forwardDirection = (From::Offset < To::Offset) ? true : false;
				if(forwardDirection) {

					auto realOffset = To::Offset - From::Offset;
					return OutputIterator(address + realOffset * sizeof(To::NodeSize));

				} else {

					auto realOffset = From::Offset - To::Offset;
					return OutputIterator(address - realOffset * sizeof(To::NodeSize));

				}
				
			} else {

				static_assert(sameTypes, "convertIterator supports only one-node-typed iterators");

			}
		}


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

