#include "output_handler.h"
#include "sorter.h"
#include "writer.h"
#include <iostream>
#include <algorithm>
#include <varcode.h>

namespace OutputHandler {


template<typename Input>
typename RankingHandler<Input>::ResultType RankingHandler<Input>::metricsCalculate(typename Input::IteratorType begin, typename Input::IteratorType end, typename Input::Traits::HashType pivotHash)
{

    tf.clear();
    df.clear();

    auto raw = begin;
    for(; raw != end; ++raw) {

        auto hash = std::get<Input::Traits::Hash>(*raw);
        auto docId = std::get<Input::Traits::DocId>(*raw);

        if(hash > pivotHash) {

            break;

        }

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

    return raw;
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

    std::wcout << "Prepare dictionary and coordinate file indexes ...\n";

	size_t coordBlockOffsetBegin = 0;
	size_t currentHash = std::get<Input::Traits::Hash>(*input.begin());
	std::vector<typename Input::Traits::DocIdType> unpackedCoordFile;


	RankingHandler<Input> ranker;
	auto rangeStart = ranker.metricsCalculate(std::cbegin(input), std::cend(input), currentHash);

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
		    auto bytesCount = std::size(wrappedBytes);

            Writer::write("../index_files/dict.bin", std::vector<size_t>{currentHash, coordBlockOffsetBegin, bytesCount});
            Writer::write("../index_files/coord.bin", std::move(wrappedBytes));

            currentHash = nextHash;
            coordBlockOffsetBegin += sizeof(unsigned char) * bytesCount;
            unpackedCoordFile.clear();

            rangeStart = ranker.metricsCalculate(rangeStart, std::cend(input), currentHash);
            --raw;

		}
	}

    auto wrappedBytes = Varcode::compress(unpackedCoordFile);
    auto bytesCount = std::size(wrappedBytes);

    Writer::write("../index_files/dict.bin", std::vector<size_t>{currentHash, coordBlockOffsetBegin, bytesCount});
    Writer::write("../index_files/coord.bin", std::move(wrappedBytes));
	
}


void StandartHandler::prepareInvCoordFile(Output& output) {

    std::wcout << "Prepare inverse coordinate index ... \n";

	size_t docIdCount = std::size(InputHandler::Output::articles);
	OutputType::InvCoordFile::HeadType bodySize = sizeof(OutputType::InvCoordFile::BodyType::value_type) * 
											  	  OutputType::InvCoordFile::bodyBlockSize *
											      docIdCount;

	size_t bottomOffset = bodySize + sizeof(OutputType::InvCoordFile::BodyType::value_type);
	size_t urlAndNameOffset = 0;

	// write head
	Writer::write("../index_files/invCoord.bin", bodySize);

	// write body
	for(auto raw = std::cbegin(InputHandler::Output::articles);raw != std::cend(InputHandler::Output::articles); ++raw) {

	    auto& [docId, information] = *raw;
	    auto& [name, url] = information;
		bottomOffset += urlAndNameOffset;

		Writer::write("../index_files/invCoord.bin", std::vector<size_t>{docId, bottomOffset, name.size(), url.size()});
        urlAndNameOffset = (name.size() + url.size()) * sizeof(OutputType::InvCoordFile::BottomType::value_type);

	}

	// writer bottom
	size_t batchSize{10000};
	std::vector<std::wstring> batch(batchSize);
    std::size_t currentPosition = 0;

	for(auto raw = std::begin(InputHandler::Output::articles);raw != std::end(InputHandler::Output::articles); ++raw) {

        auto& [docId, information] = *raw;
        auto& [name, url] = information;

        batch[currentPosition++] = std::move(name);
        batch[currentPosition++] = std::move(url);

	    if(currentPosition == batchSize) {

	        Writer::write("../index_files/invCoord.bin", std::move(batch));
	        currentPosition = 0;

	    }

	}

	if(currentPosition > 0) {

	    batch.resize(currentPosition);
        Writer::write("../index_files/invCoord.bin", std::move(batch));

	}

}

using NodeType = typename InputHandler::Output::value_type;

template class RankingHandler<typename Sorter::ExternalSorter::OutputType<NodeType>>;

template StandartHandler::OutputType 
StandartHandler::prepareForWrite<typename Sorter::ExternalSorter::OutputType<NodeType>>(typename Sorter::ExternalSorter::OutputType<NodeType>&& input);
}
