#include <search.h>
#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <functional>

int main() {
	
	boost::interprocess::file_mapping mfile("../index/files/dict.bin", boost::interprocess::read_write);
	boost::interprocess::mapped_region region(mfile, boost::interprocess::read_write);

	boost::interprocess::file_mapping mfile1("../index/files/coord.bin", boost::interprocess::read_write);
	boost::interprocess::mapped_region region1(mfile1, boost::interprocess::read_write);

	std::setlocale(LC_ALL, "ru_RU.utf8");
	std::wstring s;
	std::wcin >> s;
	auto p = std::hash<std::wstring>{}(s);
	auto addr = static_cast<size_t*>(region.get_address());
	while(p != *addr) {
		addr += 3;
	}
	auto offset = *(addr + 2);
	std::cout << offset << std::endl;
	auto coord_ptr = static_cast<size_t*>(region1.get_address());
	std::cout<<*coord_ptr<<std::endl;


	return 0;
}
