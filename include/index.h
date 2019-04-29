#pragma once 

#include <vector>
#include <string>
#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

class Index {
public:
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



