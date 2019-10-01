#pragma once

#include <string>

template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
class Indexer : public Reader, public InputHandler, public Sorter, public OutputHandler, public Writer {
private:

	std::string tokensFile;
	std::string dictFile;
	std::string coordFile;
	std::string invCoordFile;

	void cacheOnDisk(typename InputHandler::OutputType&&, const char*) const;
public:

	Indexer(const std::string&, const std::string&, const std::string&, const std::string&);
	bool make();
	~Indexer() = default;
};
