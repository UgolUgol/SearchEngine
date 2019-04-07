#include "sorter.h"
#include <algorithm>
#include <vector>
#include <tuple>

using HashType = size_t;
using DocId = size_t;
using Name = std::wstring;
using URL = std::wstring;
using PositionType = size_t;

template<typename InputType>
typename Sorter<InputType>::OutputType Sorter<InputType>
::sort(InputType& input) {

		std::sort(input.begin(), input.end(),
		[](const typename InputType::value_type& lhs,
		   const typename InputType::value_type& rhs) {
			return std::get<0>(lhs) < std::get<0>(rhs);
		});
		return std::move(input);

}

template class Sorter<std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>>;