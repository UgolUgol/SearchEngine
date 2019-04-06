#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include <locale>
#include <codecvt>
#include <future>
#include <cmath>
#include <chrono>
#include <tools.h>
#include <index_maker.h>
#include <cstring>
#include <sstream>
#include <unistd.h>

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	std::wifstream ifs("tokens");
	ifs.imbue(std::locale("ru_RU.utf8"));
	while(!ifs.eof()) {
		std::wstring info;
		std::wstring text;

		std::getline(ifs, info);
		
		if(info.empty()) break;
		
		std::vector<std::wstring> articleInfo;
		Tools::split_regex(articleInfo, info.c_str(), L"|", info.size());
		
		std::wistringstream ss(articleInfo[2]);
		size_t size = 0;
		ss >> size;
		std::wcout<<articleInfo[0]<< " " << articleInfo[1] << " " << size<<std::endl;
		text.assign(size - 1, L' ');
		ifs.read(&text[0], size);
		std::wcout<<text<<std::endl;

	}


	ifs.close();

}
