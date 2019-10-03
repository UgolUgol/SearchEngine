#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <tuple>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <optional>
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

	std::tuple<std::vector<size_t>, std::vector<unsigned char>, InvCoordFile> data;
	using Traits = TypeTraits<std::tuple<std::vector<size_t>, std::vector<unsigned char>, InvCoordFile>>;
};


template<typename Input>
class RankingHandler {
public:
    using TFKeyType = std::pair<typename Input::Traits::HashType, typename Input::Traits::DocIdType>;
    using DFKeyType = typename Input::Traits::HashType;
    using NumericType = typename Input::Traits::DocIdType;
    using ResultType = typename Input::IteratorType;

    ResultType metricsCalculate(typename Input::IteratorType begin, typename Input::IteratorType end, typename Input::Traits::HashType hash);
    std::optional<NumericType> getTf(typename Input::Traits::HashType hash, typename Input::Traits::DocIdType) const;
    std::optional<NumericType> getDf(typename Input::Traits::HashType hash) const;

private:


    struct TFComparator {

        std::size_t operator()(const TFKeyType& key) const noexcept
        {

            std::size_t seed{};
            boost::hash_combine(seed, key.first);
            boost::hash_combine(seed, key.second);

            return seed;

        }
    };

    std::unordered_map<TFKeyType, NumericType, TFComparator> tf;
    std::unordered_map<DFKeyType, NumericType> df;

};


class StandartHandler {
public:

	using OutputType = Output;
	template<typename Input> OutputType prepareForWrite(Input&& input);
private:

	template<typename Input> void prepareIndex(Input& input, Output& output);
	void prepareInvCoordFile(Output& output);
};

	
}

