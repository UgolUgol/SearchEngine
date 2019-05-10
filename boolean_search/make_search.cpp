#include <search.h>
#include <iostream>
#include <functional>
#include <set>
#include <search_input_transformator.h>

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	Search::RequestHandler req;
	InputTransformator<std::wstring> t;

	std::wstring s;
	std::getline(std::wcin, s);
	auto result = req.search(t.transform(s));	
	for(const auto& block : result) {
		std::wcout<<block.name<<std::endl<<block.url<<std::endl<<std::endl;
	}

	return 0;
}
