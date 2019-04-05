#include "index_maker.h"

template<typename Reader, typename Sorter, typename Writer>
Indexer<Reader, Sorter, Writer>::Indexer(const std::string& tokensFile, 
										 const std::string& dictFile,
										 const std::string& coordFile,
										 const std::string& invCoordFile) :
										 tokensFile(tokensFile),
										 dictFile(dictFile),
										 coordFile(coordFile),
										 invCoordFile(invCoordFile) { }


template<typename Reader, typename Sorter, typename Writer>
bool Indexer<Reader, Sorter, Writer>::make() {

	Reader::openFile(tokensFile);
	ReadResult data;
	while(!Reader::fileEnd()) {

		data += Reader::read();

	}
	Reader::closeFile();

	Writer::openFiles()
	SortResult sortedData = Sorter::sort(data);

	return Writer::write(sortedData);
}
