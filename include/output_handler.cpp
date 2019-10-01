#include "output_handler.h"
#include "sorter.h"
#include <iostream>
#include <algorithm>
#include <varcode.h>

namespace OutputHandler {


template<typename Input>
void RankingHandler<Input>::metricsCalculate(const Input &data)
{

    for(auto& raw : data) {

        auto hash = std::get<Input::Traits::Hash>(raw);
        auto docId = std::get<Input::Traits::DocId>(raw);

        auto [tfNode, tfWasInserted] = tf.emplace(std::piecewise_construct,
                                                  std::forward_as_tuple(hash, docId),
                                                  std::forward_as_tuple(1));

        if(tfWasInserted) {

            auto [dfNode, dfWasInserted] = df.emplace(hash, 1);
            if(!dfWasInserted) {

                ++(dfNode->second);

            }

        }

        if(!tfWasInserted) {

            ++(tfNode->second);

        }

    }
}

template<typename Input>
std::optional<typename RankingHandler<Input>::NumericType>
RankingHandler<Input>::getTf(typename Input::Traits::HashType hash, typename Input::Traits::DocIdType docId) const
{

    auto tfNode = tf.find(std::make_pair(hash, docId));
    if(tfNode == std::end(tf)) {

        return std::nullopt;

    }

    return tfNode->second;

}

template<typename Input>
std::optional<typename RankingHandler<Input>::NumericType>
RankingHandler<Input>::getDf(typename Input::Traits::HashType hash) const
{

    auto dfNode = df.find(hash);
    if(dfNode == std::end(df)) {

        return std::nullopt;

    }

    return dfNode->second;

}


template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForWrite(Input&& input) {


	if(std::cbegin(input) == std::cend(input)) {
		return {};
	}

	OutputType output;

	prepareIndex(input, output);
	prepareInvCoordFile(output);

	return output;
}


template<typename Input>
void StandartHandler::prepareIndex(Input& input, Output& output) {

	decltype(auto) dictFile = std::get<OutputType::Traits::DictFile>(output.data);
	decltype(auto) coordFile = std::get<OutputType::Traits::CoordFile>(output.data);

	size_t coordBlockOffsetBegin = 0;
	size_t currentHash = std::get<Input::Traits::Hash>(*input.begin());
	std::vector<typename Input::Traits::DocIdType> unpackedCoordFile;


	RankingHandler<Input> ranker;
	ranker.metricsCalculate(input);

	for(auto raw = input.begin(); raw != input.end(); ++raw) {

		size_t nextHash = std::get<Input::Traits::Hash>(*raw);
		if(currentHash == nextHash) {

			auto docId = std::get<Input::Traits::DocId>(*raw);
			auto position = std::get<Input::Traits::Position>(*raw);
            auto tf = ranker.getTf(nextHash, docId);
            auto df = ranker.getDf(nextHash);

			unpackedCoordFile.insert(std::end(unpackedCoordFile), {docId, position, *tf, *df});
		
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


void StandartHandler::prepareInvCoordFile(Output& output) {

	decltype(auto) invCoordFile = std::get<OutputType::Traits::InvCoordFile>(output.data);

	size_t docIdCount = std::size(InputHandler::Output::articles);
	OutputType::InvCoordFile::HeadType bodySize = sizeof(OutputType::InvCoordFile::BodyType::value_type) * 
											  	  OutputType::InvCoordFile::bodyBlockSize *
											      docIdCount;

	size_t bottomOffset = bodySize + sizeof(OutputType::InvCoordFile::BodyType::value_type);
	size_t urlAndNameOffset = 0;
	for(auto raw = std::cbegin(InputHandler::Output::articles);raw != std::cend(InputHandler::Output::articles); ++raw) {

	    auto& [docId, information] = *raw;
	    auto& [name, url] = information;
		bottomOffset += urlAndNameOffset;
		
		invCoordFile.body.insert(invCoordFile.body.end(), {docId, bottomOffset, name.size(), url.size()});
		invCoordFile.bottom += (name + url);
		urlAndNameOffset = (name.size() + url.size()) * sizeof(OutputType::InvCoordFile::BottomType::value_type);

	}
	invCoordFile.head = bodySize;

}

using NodeType = typename InputHandler::Output::value_type;

template class RankingHandler<typename Sorter::ExternalSorter::OutputType<NodeType>>;

template StandartHandler::OutputType 
StandartHandler::prepareForWrite<typename Sorter::ExternalSorter::OutputType<NodeType>>(typename Sorter::ExternalSorter::OutputType<NodeType>&& input);
}
