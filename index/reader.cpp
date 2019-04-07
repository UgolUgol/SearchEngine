#include "reader.h"
#include <tools.h>
#include <sstream>
#include <vector>
#include <iostream>



enum HeadLine : size_t { Name = 0, Url, Size};

StandartReader::StandartReader() {
	ifs.imbue(std::locale("ru_RU.utf8"));
}
StandartReader::~StandartReader() {
	ifs.close();
}

bool StandartReader::openFile(const std::string& filename) {
	ifs.open(filename);
	return ifs.is_open();
}

bool StandartReader::isFileEnd() {
	return ifs.eof();
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

	return std::make_tuple(vHeadLine[HeadLine::Name], vHeadLine[HeadLine::Url], text);
}