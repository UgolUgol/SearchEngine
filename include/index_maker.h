#pragma once

#include <string>

template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
class Indexer : public Reader, public InputHandler, public Sorter, public OutputHandler, public Writer {
	std::string tokensFile;
	std::string dictFile;
	std::string coordFile;
	std::string invCoordFile;
	
	using SortInput = typename InputHandler::Result;
	using WriteInput = typename OutputHandler::Result;
public:

	Indexer(const std::string&, const std::string&, const std::string&, const std::string&);
	bool make();
	~Indexer() = default;
};
