#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <tuple>

using HashType = size_t;
using DocId = size_t;
using Name = std::wstring;
using URL = std::wstring;
using PositionType = size_t;

class Sorter {
public:
	using OutputType = std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>;
	template<typename InputType> OutputType sort(InputType& input);
};