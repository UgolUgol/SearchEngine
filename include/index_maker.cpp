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

	typename Reader::OutputType data;
	typename InputHandler::SortInput transformedData; 

	Reader::openFile(tokensFile);
	while(!Reader::fileEnd()) {

		data += Reader::read();
		transformedData += InputHandler::prepareForSort(data);

	}
	typename Sorter::OutputType sortedData = Sorter::sort(data);

	Writer::openFiles(dictFile, coordFile, invCoordFile);
	return Writer::write(OutputHandler::prepareForWrite(sortedData));
}
