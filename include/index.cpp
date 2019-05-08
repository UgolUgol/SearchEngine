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
typename Index<IndexType>::OffsetInfo::Type 
Index<IndexType>::getOffset(typename Index<IndexType>::HashIterator iterator) const {

	auto offset = std::distance(dictionaryBegin<Hash>(), iterator);
	return *(dictionaryBegin<OffsetInfo>() + offset);

}


template<typename IndexType> 
typename Index<IndexType>::LengthInfo::Type 
Index<IndexType>::getLength(typename Index<IndexType>::HashIterator iterator) const {

	auto offset = std::distance(dictionaryBegin<Hash>(), iterator);
	return *(dictionaryBegin<LengthInfo>() + offset);

}


template<typename IndexType>
DirectIndex<IndexType>::DirectIndex(const char* invCoordFile) :
									invCoord(invCoordFile, boost::interprocess::read_write),
									mappedInvCoord(invCoord, boost::interprocess::read_write) { }

template class Index<DefaultIndex>;
template class DirectIndex<DefaultIndex>;