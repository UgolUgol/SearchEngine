#include "output_handler.h"
#include "sorter.h"
#include <iostream>
#include <algorithm>

namespace OutputHandler {

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForWrite(Input&& input) {


	if(input.size() == 0) {
		return {};
	}
	OutputType output;

	prepareIndex(input, output);
	//prepareInvCoordFile(input, output);

	return output;
}


template<typename Input>
void StandartHandler::prepareIndex(Input& input, Output& output) {

	decltype(auto) dictFile = std::get<OutputType::Traits::DictFile>(output.data);
	decltype(auto) coordFile = std::get<OutputType::Traits::CoordFile>(output.data);

	size_t coordBlockOffsetBegin = 0;
	size_t coordBlockOffsetLength= 0;
	size_t currentHash = std::get<Input::Traits::Hash>(*input.begin());

	for(auto raw = input.begin(); raw != input.end(); ++raw) {

		size_t nextHash = std::get<Input::Traits::Hash>(*raw);
		if(currentHash == nextHash) {

			auto docId = std::get<Input::Traits::DocId>(*raw);
			auto position = std::get<Input::Traits::Position>(*raw);

			++coordBlockOffsetLength; 

			coordFile.insert(coordFile.end(), {docId, position});
		
		} else {

			dictFile.insert(dictFile.end(), {currentHash, coordBlockOffsetBegin, coordBlockOffsetLength});
			currentHash = nextHash;
			coordBlockOffsetBegin += sizeof(size_t) * coordFile.size();
			coordBlockOffsetLength = 0;
			--raw;
		}
	}
	dictFile.insert(dictFile.end(), {currentHash, coordBlockOffsetBegin, coordBlockOffsetLength});
}


template<typename Input>
void StandartHandler::prepareInvCoordFile(Input& input, Output& output) {

	decltype(auto) invCoordFile = std::get<OutputType::Traits::InvCoordFile>(output.data);

	std::sort(input.begin(), input.end(), [](const auto& lhs, const auto& rhs) {
		return std::get<Input::Traits::DocId>(lhs) < std::get<Input::Traits::DocId>(rhs);
	});

	auto uniqueEnd = std::unique(input.begin(), input.end(), [](const auto& lhs, const auto& rhs) {
		return std::get<Input::Traits::DocId>(lhs) == std::get<Input::Traits::DocId>(rhs);
	});

	for(auto raw = input.begin(); raw != uniqueEnd; ++raw) {


	}
}

template StandartHandler::OutputType 
StandartHandler::prepareForWrite<typename Sorter::QuickSorter::OutputType>(typename Sorter::QuickSorter::OutputType&& input);
}
