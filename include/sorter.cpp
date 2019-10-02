#include "sorter.h"
#include <algorithm>
#include <iostream>

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

template<typename NodeType>
DiskCachedData<NodeType> ExternalSorter::sort(DiskCachedData<NodeType> &input)
{

    std::wcout << "Start sorting...\n";

    std::stable_sort(std::begin(input), std::end(input),
            [](const NodeType& lhs, const NodeType& rhs) {
       return std::get<0>(lhs) < std::get<0>(rhs);
    });

    return std::move(input);

}

template typename QuickSorter::OutputType QuickSorter::sort<InputHandler::Output>(InputHandler::Output& input);
template ExternalSorter::OutputType<InputHandler::Output::value_type> ExternalSorter
        ::sort<InputHandler::Output::value_type>(ExternalSorter::OutputType<InputHandler::Output::value_type>&);
}
