#include "output_handler.h"
#include "sorter.h"
#include <iostream>

namespace OutputHandler {

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForWrite(Input&& input) {


	if(input.size() == 0) {
		return {};
	}

	OutputType output;

	size_t coordFileOffset = 0;;
	size_t invCoordOffset = 0;
	size_t currentHash = std::get<Input::Traits::Hash>(*input.begin());

	decltype(auto) dictFile = std::get<OutputType::Traits::DictFile>(output.data);
	decltype(auto) coordFile = std::get<OutputType::Traits::CoordFile>(output.data);
	decltype(auto) invCoordFile = std::get<OutputType::Traits::InvCoordFile>(output.data);

	for(auto raw = input.begin(); raw != input.end(); ++raw) {

		size_t nextHash = std::get<Input::Traits::Hash>(*raw);
		if(currentHash == nextHash) {

			auto docId = std::get<Input::Traits::DocId>(*raw);
			auto name = std::get<Input::Traits::Name>(*raw);
			auto url = std::get<Input::Traits::Url>(*raw);
			auto position = std::get<Input::Traits::Position>(*raw);

			coordFileOffset += (sizeof(docId) + sizeof(coordFileOffset) + sizeof(position));
			invCoordOffset += name.size() + url.size();

			coordFile.insert(coordFile.end(), {docId, invCoordOffset, position});
			invCoordFile.insert(invCoordFile.end(), {name, url});
		
		} else {

			dictFile.insert(dictFile.end(), {currentHash, coordFileOffset});
			currentHash = nextHash;
			--raw;
		}
	}
	dictFile.insert(dictFile.end(), {currentHash, coordFileOffset});
	return output;

}

template StandartHandler::OutputType 
StandartHandler::prepareForWrite<typename Sorter::QuickSorter::OutputType>(typename Sorter::QuickSorter::OutputType&& input);
}
