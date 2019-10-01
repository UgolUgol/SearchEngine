#pragma once
#include <vector>
#include <tuple>
#include <map>


namespace InputHandler {

template<typename OutputType>
struct TypeTraits {};

template<typename T>
struct TypeTraits<std::vector<T>> {

	static void concatenate(std::vector<T>& lhs, std::vector<T>&& rhs) {
		lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
	}

	using ValueType = T;
	enum Indexes : size_t { Hash=0, DocId, Position};
};


struct Output {

	using HashType = size_t;
	using DocId = size_t;
	using Name = std::wstring;
	using URL = std::wstring;
	using PositionType = size_t;
	using value_type = std::tuple<HashType, DocId, PositionType>;

    std::vector<value_type> data;
    static std::map<DocId, std::pair<Name, URL>> articles;

	using Traits = TypeTraits<std::vector<value_type>>;

	operator std::vector<std::tuple<HashType, DocId, PositionType>>&() & { return data; }
	operator std::vector<std::tuple<HashType, DocId, PositionType >>&&() && { return std::move(data); }

	auto begin() {
		return std::begin(data);
	}

	auto end() {
		return std::end(data);
	}

	auto begin() const {
	    return std::cbegin(data);
	}

	auto end() const {
	    return std::cend(data);
	}


	size_t size() {
		return data.size();
	}
};

class StandartHandler {
	static Output::DocId docId;
public:
	using OutputType = Output;
	template<typename Input> OutputType prepareForSort(Input&&);
};
	
}
