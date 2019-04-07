#include "sorter.h"
#include <algorithm>

template<typename InputType>
typename Sorter::OutputType Sorter
::sort(InputType& input) {

		std::sort(input.begin(), input.end(),
		[](const typename InputType::value_type& lhs,
		   const typename InputType::value_type& rhs) {
			return std::get<0>(lhs) < std::get<0>(rhs);
		});
		return std::move(input);

}

using InputType = std::vector<std::tuple<HashType, DocId, Name, URL, PositionType>>;
template typename Sorter::OutputType Sorter::sort<InputType>(InputType& input);