#pragma once
#include <vector>
#include <tuple>

using HashType = size_t;
using DocId = size_t;
using Name = std::wstring;
using URL = std::wstring;
using PositionType = size_t;

enum Input : size_t;


template<typename OutputType>
struct Traits {};

template<typename T>
struct Traits<std::vector<T>> {

	static void concatenate(std::vector<T>& lhs, std::vector<T>&& rhs) {
		lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}
};

class InputHandler {
	static size_t docId;
public:
	using OutputType = std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>;
	using OutputTraits = Traits<OutputType>;
	template<typename InputType> OutputType prepareForSort(InputType&&);
};
