#pragma once
#include <vector>
#include <tuple>


namespace InputHandler {

using HashType = size_t;
using DocId = size_t;
using Name = std::wstring;
using URL = std::wstring;
using PositionType = size_t;

template<typename OutputType>
struct TypeTraits {};

template<typename T>
struct TypeTraits<std::vector<T>> {

	static void concatenate(std::vector<T>& lhs, std::vector<T>&& rhs) {
		lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}

	using ValueType = T;
	enum Indexes : size_t { Hash=0, DocId, Name, Url, PositionType };
};


struct Output {
	std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>> data;
	using Traits = TypeTraits<std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>>;

	operator std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>&() & { return data; }
	operator std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>&&() && { return std::move(data); }

	auto begin() {
		return data.begin();
	}
	auto end() {
		return data.end();
	}
};

class StandartHandler {
	static size_t docId;
public:
	using OutputType = Output;
	template<typename Input> OutputType prepareForSort(Input&&);
};
	
}
