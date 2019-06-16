#include "index.h"
#include <algorithm>


template<typename IndexType>
DictionaryIndex<IndexType>::DictionaryIndex(const char* dictFile) : 
							dict(dictFile, boost::interprocess::read_write),
							mappedDict(dict, boost::interprocess::read_write) { }

template<typename IndexType>
CoordinateIndex<IndexType>::CoordinateIndex(const char* coordFile) : 
							coord(coordFile, boost::interprocess::read_write),
							mappedCoord(coord, boost::interprocess::read_write) { }


template<typename IndexType>
DirectIndex<IndexType>::DirectIndex(const char* invCoordFile) :
						invCoord(invCoordFile, boost::interprocess::read_write),
						mappedInvCoord(invCoord, boost::interprocess::read_write) { }

template class Index<DefaultIndex>;
template class DictionaryIndex<DefaultIndex>;
template class CoordinateIndex<DefaultIndex>;
template class DirectIndex<DefaultIndex>;