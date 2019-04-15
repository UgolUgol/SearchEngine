#include "sorter.h"
#include <algorithm>

namespace Sorter {

template<typename Input>
QuickSorter::OutputType QuickSorter::sort(Input& input) {

		std::stable_sort(input.begin(), input.end(),
		[](const typename Input::Traits::ValueType& lhs,
		   const typename Input::Traits::ValueType& rhs) {
			return std::get<Input::Traits::Hash>(lhs) < std::get<Input::Traits::Hash>(rhs);
		});
		return std::move(input);

}

template typename QuickSorter::OutputType QuickSorter::sort<InputHandler::Output>(InputHandler::Output& input);
}
