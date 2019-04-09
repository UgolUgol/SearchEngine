#pragma once

#include <string>
#include <fstream>
#include <tuple>


namespace Reader {

enum HeadLine: size_t;

template<typename Type> 
struct TypeTraits { };

template<typename T1, typename T2, typename T3>
struct TypeTraits<std::tuple<T1, T2, T3>>{
	
	using NameType = T1;
	using UrlType = T2;
	using TokensType = T3;

	enum Indexes : size_t { Name=0, Url, Tokens };
};

struct Output {
	std::tuple<std::wstring, std::wstring, std::wstring> data;
	using Traits = TypeTraits<std::tuple<std::wstring, std::wstring, std::wstring>>;
};

class StandartReader {
	std::wifstream ifs;
	unsigned readSize;
	unsigned totalSize;
public:
	using OutputType = Output; 

	StandartReader();
	~StandartReader();

	bool openFile(const std::string& filename);
	bool isFileEnd();
	OutputType read();

};
	
};


