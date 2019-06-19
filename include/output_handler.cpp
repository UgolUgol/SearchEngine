#include "output_handler.h"
#include "sorter.h"
#include <iostream>
#include <algorithm>
#include <varcode.h>

namespace OutputHandler {

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForWrite(Input&& input) {


	if(input.size() == 0) {
		return {};
	}
	OutputType output;

	prepareIndex(input, output);
	prepareInvCoordFile(input, output);

	return output;
}


template<typename Input>
void StandartHandler::prepareIndex(Input& input, Output& output) {

	decltype(auto) dictFile = std::get<OutputType::Traits::DictFile>(output.data);
	decltype(auto) coordFile = std::get<OutputType::Traits::CoordFile>(output.data);

	size_t coordBlockOffsetBegin = 0;
	size_t currentHash = std::get<Input::Traits::Hash>(*input.begin());
	std::vector<typename Input::DocId> unpackedCoordFile;

	for(auto raw = input.begin(); raw != input.end(); ++raw) {

		size_t nextHash = std::get<Input::Traits::Hash>(*raw);
		if(currentHash == nextHash) {

			auto docId = std::get<Input::Traits::DocId>(*raw);
			auto position = std::get<Input::Traits::Position>(*raw);

			unpackedCoordFile.insert(std::end(unpackedCoordFile), {docId, position});
		
		} else {

			auto wrappedBytes = Varcode::compress(unpackedCoordFile); 
			coordFile.insert(std::end(coordFile), std::begin(wrappedBytes), std::end(wrappedBytes));
			dictFile.insert(dictFile.end(), {currentHash, coordBlockOffsetBegin, wrappedBytes.size()});
			
			currentHash = nextHash;
			coordBlockOffsetBegin = sizeof(unsigned char) * coordFile.size();
			unpackedCoordFile.resize(0);
			--raw;
		}
	}
	auto wrappedBytes = Varcode::compress(unpackedCoordFile); 
	coordFile.insert(std::end(coordFile), std::begin(wrappedBytes), std::end(wrappedBytes));
	dictFile.insert(dictFile.end(), {currentHash, coordBlockOffsetBegin, wrappedBytes.size()});
	
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


	size_t docIdCount = std::distance(input.begin(), uniqueEnd);
	OutputType::InvCoordFile::HeadType bodySize = sizeof(OutputType::InvCoordFile::BodyType::value_type) * 
											  	  OutputType::InvCoordFile::bodyBlockSize * 
											      docIdCount;

	size_t bottomOffset = bodySize + sizeof(OutputType::InvCoordFile::BodyType::value_type);
	size_t urlAndNameOffset = 0;
	for(auto raw = input.begin(); raw != uniqueEnd; ++raw) {
			
		auto name = std::get<Input::Traits::Name>(*raw);
		auto url = std::get<Input::Traits::Url>(*raw);
		auto docId = std::get<Input::Traits::DocId>(*raw);
		bottomOffset += urlAndNameOffset;

		invCoordFile.body.insert(invCoordFile.body.end(), {docId, bottomOffset, name.size(), url.size()});
		invCoordFile.bottom += (name + url);
		urlAndNameOffset = (name.size() + url.size()) * sizeof(OutputType::InvCoordFile::BottomType::value_type);

	}
	invCoordFile.head = bodySize;

}

template StandartHandler::OutputType 
StandartHandler::prepareForWrite<typename Sorter::QuickSorter::OutputType>(typename Sorter::QuickSorter::OutputType&& input);
}
