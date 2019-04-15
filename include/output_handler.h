#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <tuple>


namespace OutputHandler {

template<typename T> 
struct TypeTraits { };

template<typename T1, typename T2, typename T3>
struct TypeTraits<std::tuple<std::vector<T1>, std::vector<T2>, T3>> {

	using DictType = T1;
	using CoordType = T2;
	using InvCoordType = T3;

	enum Indexer : size_t { DictFile=0, CoordFile, InvCoordFile };
};

struct Output {

	struct InvCoordFile {

		using HeadType = size_t;
		using BodyType = std::vector<size_t>;
		using BottomType = std::wstring;

		static constexpr size_t bodyBlockSize = 4;

		HeadType head;
		BodyType body;
		BottomType bottom;

	};

	std::tuple<std::vector<size_t>, std::vector<size_t>, InvCoordFile> data;
	using Traits = TypeTraits<std::tuple<std::vector<size_t>, std::vector<size_t>, InvCoordFile>>;
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

