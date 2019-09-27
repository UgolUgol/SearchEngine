#pragma once

#include <string>
#include <fstream>
#include <tuple>
#include <optional>


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
	static constexpr wchar_t separator[] = L"\n";
	using Traits = TypeTraits<std::tuple<std::wstring, std::wstring, std::wstring>>;
};

class StandartReader {

	std::wifstream ifs;
	uint64_t readSize;
	uint64_t totalSize;
    Output::Traits::TokensType headLine;

public:
	using OutputType = Output; 

	StandartReader();
	~StandartReader();

	bool openFile(const std::string& filename);
	bool isFileEnd();
	OutputType read();

};
	
};


