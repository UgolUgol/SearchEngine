#include <search.h>
#include <iostream>
#include <functional>
#include <set>


int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	Search::RequestHandler req;
	std::wstring s;
	std::getline(std::wcin, s);

	auto result = req.search(s);	
	for(const auto& block : result) {
		std::wcout<<block.name<<std::endl<<block.url<<std::endl<<std::endl;
	}

	return 0;
}
