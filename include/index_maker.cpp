#include "index_maker.h"
#include "reader.h"
#include "input_handler.h"
#include "sorter.h"
#include "output_handler.h"
#include "writer.h"

template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
Indexer<Reader, InputHandler, Sorter, OutputHandler, Writer>
::Indexer(const std::string& tokensFile, 
		  const std::string& dictFile,
		  const std::string& coordFile,
		  const std::string& invCoordFile) :
		  tokensFile(tokensFile),
		  dictFile(dictFile),
		  coordFile(coordFile),
		  invCoordFile(invCoordFile) { }


template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
bool Indexer<Reader, InputHandler, Sorter, OutputHandler, Writer>::make() {

	typename InputHandler::OutputType data; 

	Reader::openFile(tokensFile);
	while(!Reader::isFileEnd()) {

		InputHandler::OutputType::Traits::concatenate(data, InputHandler::prepareForSort(Reader::read()));

	}
	
	Writer::openFiles(dictFile, coordFile, invCoordFile);
	return Writer::write(OutputHandler::prepareForWrite(Sorter::sort(data)));
}

template class Indexer<Reader::StandartReader,
 					   InputHandler::StandartHandler,
 					   Sorter::QuickSorter,
 					   OutputHandler::StandartHandler,
 					   Writer::StandartWriter>;
