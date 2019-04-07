#pragma once
#include <vector>
#include <tuple>

using HashType = size_t;
using DocId = size_t;
using Name = std::wstring;
using URL = std::wstring;
using PositionType = size_t;

enum Input : size_t;

class InputHandler {
	static size_t docId;
public:
	using OutputType = std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>;
	
	template<typename InputType> OutputType prepareForSort(InputType&&);
	OutputType operator+(const OutputType& rhs);
};
