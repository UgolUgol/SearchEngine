#include <search.h>
#include <iostream>
#include <functional>
#include <set>
#include <search_input_transformator.h>
#include <chrono>
#include <stemmer.h>
#include <unordered_map>
#include <vector>
#include <csignal>
#include <tuple>
#include <fstream>

template<typename T>
void f(T) {
    std::cout <<"first"<<std::endl;
}

template<typename T>
void f(T*)
{
    std::cout << "second" << std::endl;
}

template<>
void f<int*>(int*) {
    std::cout <<"third"<<std::endl;
}



int main() {

	std::setlocale(LC_ALL, "ru_RU.utf8");

	Search::RequestHandler req;
	InputTransformator<std::wstring> t;

	std::wstring s;
	std::getline(std::wcin, s);

	auto transformedExpression = t.transform(s);
	auto result = req.search(transformedExpression);
	for(const auto& block : result) {
		std::wcout<<block.name<<std::endl<<block.url<<std::endl<<std::endl;
	}

	std::wcout<<"Results for request: "<<transformedExpression<<std::endl;
	std::wcout<<"Results count: "<<result.size()<<std::endl;

	return 0;

}
