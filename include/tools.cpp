#include "tools.h"
#include <fstream>


namespace Tools {
	
	std::wstring readData(std::string filename) {

		std::wifstream ifs(filename);
		ifs.imbue(std::locale("ru_RU.utf8"));

		ifs.seekg(0, std::ios::end);
		size_t size = ifs.tellg();
		std::wstring texts(size, L' ');
		ifs.seekg(0, std::ios::beg);
		
		ifs.read(&texts[0], size);
		ifs.close();

		return texts;
	}


	void split_regex(Tokens& dest, const wchar_t* src, const wchar_t* breakset, size_t srcSize) {

		const wchar_t* end = src + srcSize;
		while(src != end) {
			auto str = std::wcspbrk(src, breakset);
			if(!str) {
				dest.emplace_back(src);
				break;
			}

			size_t diff = str - src;
			if(diff) {
				dest.emplace_back(src, diff);
			}
			src = str + 1;
		}
	}
}
