#include "sorter.h"
#include <algorithm>
#include "input_handler.h"

template<typename Input>
typename Sorter::OutputType Sorter
::sort(Input& input) {

		std::sort(input.begin(), input.end(),
		[](const typename Input::Traits::ValueType& lhs,
		   const typename Input::Traits::ValueType& rhs) {
			return std::get<Input::Traits::Hash>(lhs) < std::get<Input::Traits::Hash>(rhs);
		});
		return std::move(input);

}

template typename Sorter::OutputType Sorter::sort<InputHandler::Output>(InputHandler::Output& input);
