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

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	std::wifstream ifs("a");
	ifs.imbue(std::locale("ru_RU.utf8"));

	while(!ifs.eof()) {
		std::wstring info;
		std::wstring text;

		std::getline(ifs, info);
		std::wistringstream ss(info);
		
		if(info.empty()) break;
		size_t size;
		ss >> size;

		text.assign(size - 1, L' ');
		ifs.read(&text[0], size);
		std::wcout<<text<<std::endl;
		std::wcout<<"===="<<std::endl;
		
	}


	ifs.close();

}
