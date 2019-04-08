#include "reader.h"
#include <tools.h>
#include <sstream>
#include <vector>
#include <iostream>



enum HeadLine : size_t { Name = 0, Url, Size};

StandartReader::StandartReader() : readSize(0), totalSize(0) {
	ifs.imbue(std::locale("ru_RU.utf8"));
}
StandartReader::~StandartReader() {
	ifs.close();
}

bool StandartReader::openFile(const std::string& filename) {
	ifs.open(filename);

	std::wstring fileSize;
	std::getline(ifs, fileSize);
	std::wistringstream ss(fileSize);
	ss >> totalSize;

	return ifs.is_open();
}

bool StandartReader::isFileEnd() {
	return readSize >= totalSize;
}

typename StandartReader::OutputType StandartReader::read() {

	std::wstring text;
	std::wstring headLine;
	std::vector<std::wstring> vHeadLine; 
	std::getline(ifs, headLine);

	Tools::split_regex(vHeadLine, headLine.c_str(), L"|", headLine.size());
	size_t size;
	std::wistringstream ss(vHeadLine[HeadLine::Size]);
	ss >> size;
	text.assign(size - 1, L' ');
	ifs.read(&text[0], size);

	readSize += size;
	return std::make_tuple(vHeadLine[HeadLine::Name], vHeadLine[HeadLine::Url], text);
}
