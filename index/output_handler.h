#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <tuple>


namespace OutputHandler {

template<typename T> 
struct TypeTraits { };

template<typename T1, typename T2, typename T3>
struct TypeTraits<std::tuple<std::vector<T1>, std::vector<T2>, std::vector<T3>>> {

	using DictType = T1;
	using CoordType = T2;
	using InvCoordType = T3;

	enum Indexer : size_t { DictFile=0, CoordFile, InvCoordFile };
};

struct Output {

	std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<std::wstring>> data;
	using Traits = TypeTraits<std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<std::wstring>>>;

};

class StandartHandler {
public:

	using OutputType = Output;

	template<typename Input> 
	OutputType prepareForWrite(Input&& input);
private:

	template<typename Input> void prepareIndex(Input& input, Output& output);
	template<typename Input> void prepareInvCoordFile(Input& input, Output& output);
};
	
}

