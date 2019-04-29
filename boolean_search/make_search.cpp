#include <search.h>
#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <functional>
#include <index.h>

int main() {
	
	Index idx("../index/files/dict.bin", "../index/files/coord.bin", "../index/files/invCoord.bin");
	boost::interprocess::file_mapping mfile("../index/files/dict.bin", boost::interprocess::read_write);
	boost::interprocess::mapped_region region(mfile, boost::interprocess::read_write);

	boost::interprocess::file_mapping mfile1("../index/files/coord.bin", boost::interprocess::read_write);
	boost::interprocess::mapped_region region1(mfile1, boost::interprocess::read_write);


	boost::interprocess::file_mapping mfile2("../index/files/invCoord.bin", boost::interprocess::read_write);
	boost::interprocess::mapped_region region2(mfile2, boost::interprocess::read_write);

	//std::wcout.sync_with_stdio(false);
	std::setlocale(LC_ALL, "ru_RU.utf8");
	std::wstring s;
	std::wcin >> s;
	auto p = std::hash<std::wstring>{}(s);
	auto addr = static_cast<size_t*>(region.get_address());
	while(p != *addr) {
		addr += 3;
	}
	auto offset = *(addr + 1) / sizeof(size_t);
	auto count = *(addr + 2);
	
	auto coord_ptr = static_cast<size_t*>(region1.get_address()) + offset;
	auto invCoord_ptr = static_cast<size_t*>(region2.get_address());
	auto invCoord_beg = invCoord_ptr;
	++invCoord_ptr;
	while(coord_ptr[0] != *invCoord_ptr) {
		invCoord_ptr += 4;
	}
	
	auto invOffset = *(invCoord_ptr + 1);
	auto nameSize = *(invCoord_ptr + 2);
	auto urlSize = *(invCoord_ptr + 3);
	auto article = reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(invCoord_beg) + invOffset);
	auto url = article + nameSize;
	std::wcout<<std::wstring(article, article + nameSize) << std::endl;
	std::wcout<<std::wstring(url, url + urlSize) << std::endl;


	return 0;
}
