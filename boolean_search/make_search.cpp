#include <search.h>
#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <filesystem>

int main() {
	std::filesystem::path path = "../index/files/dict.bin";
	auto filesize = std::filesystem::file_size(path);
	std::cout<<filesize<<std::endl;
/*	boost::interprocess::file_mapping m_file("../index/files/dict.bin", read);
	boost::interprocess::mapped_region region(m_file, read);*/


	return 0;
}
