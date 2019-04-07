#include "index_maker.h"

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

	typename InputHandler::OutputType transformedData; 

	Reader::openFile(tokensFile);
	while(!Reader::fileEnd()) {

		transformedData += InputHandler::prepareForSort(Reader::read());

	}
	typename Sorter::OutputType sortedData = Sorter::sort(transformedData);

	Writer::openFiles(dictFile, coordFile, invCoordFile);
	return Writer::write(OutputHandler::prepareForWrite(sortedData));
}
