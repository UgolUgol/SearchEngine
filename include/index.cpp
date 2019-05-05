#include "index.h"
#include <algorithm>


template<typename IndexType>
Index<IndexType>::Index(const char* dictFile, 
					    const char* coordFile) : 
						dict(dictFile, boost::interprocess::read_write),
						coord(coordFile, boost::interprocess::read_write),
						mappedDict(dict, boost::interprocess::read_write),
						mappedCoord(coord, boost::interprocess::read_write) { }


template<typename IndexType> 
typename Index<IndexType>::DictionaryOffsetNodeType 
Index<IndexType>::getOffset(typename Index<IndexType>::DictionaryIterator iterator) const {

	auto offset = IndexTraits<IndexType>::Dictionary::CoordOffset::Offset;
	auto blockSize = IndexTraits<IndexType>::CoordinateFile::NodeSize;

	return (*reinterpret_cast<DictionaryOffsetNodeType*>(iterator.rawPointer() + offset)) / blockSize;

}


template<typename IndexType> 
typename Index<IndexType>::DictionaryLengthNodeType 
Index<IndexType>::getLength(typename Index<IndexType>::DictionaryIterator iterator) const {

	auto offset = IndexTraits<IndexType>::Dictionary::Length::Offset;
	return *reinterpret_cast<DictionaryLengthNodeType*>(iterator.rawPointer() + offset);

}


template<typename IndexType>
DirectIndex<IndexType>::DirectIndex(const char* invCoordFile) :
									invCoord(invCoordFile, boost::interprocess::read_write),
									mappedInvCoord(invCoord, boost::interprocess::read_write) { }

template class Index<DefaultIndex>;
template class DirectIndex<DefaultIndex>;