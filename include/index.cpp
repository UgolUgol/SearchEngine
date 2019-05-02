#include "index.h"
#include <algorithm>


template<typename NodeType, size_t DictNodeSize, size_t CoordNodeSize>
Index<NodeType, DictNodeSize, CoordNodeSize>::Index(const char* dictFile, 
										  const char* coordFile,
										  const char* invCoordFile) : 
										  dict(dictFile, boost::interprocess::read_write),
										  coord(coordFile, boost::interprocess::read_write),
										  invCoord(invCoordFile, boost::interprocess::read_write),
										  mappedDict(dict, boost::interprocess::read_write),
										  mappedCoord(coord, boost::interprocess::read_write),
										  mappedInvCoord(invCoord, boost::interprocess::read_write) {

										  }

template class Index<DefaultIndex::NodeType, DefaultIndex::DictNodeSize, DefaultIndex::CoordNodeSize>;