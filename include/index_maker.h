#pragma once

template<typename Reader, typename Sorter, typename Writer>
class Indexer : public Reader, public Sorter<, public Writer {
	std::string tokensFile;
	std::string dictFile;
	std::string coordFile;
	std::string invCoordFile;
	
	using ReadResult = Reader::ReadResult;
	using SortResult = Sorter::SortResult;
public:

	Indexer(const std::string&, const std::string&, const std::string&, const std::string&);
	bool make();
	~Indexer() = default;
};
