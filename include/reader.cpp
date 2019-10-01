#include "reader.h"
#include <tools.h>
#include <sstream>
#include <vector>
#include <iostream>

namespace Reader {

enum HeadLine : size_t { Name = 0, Url, Size};

StandartReader::StandartReader() : readSize(0), totalSize(0) {
	ifs.imbue(std::locale("ru_RU.utf8"));
}
StandartReader::~StandartReader() {
	ifs.close();
}

bool StandartReader::openFile(const std::string& filename) {
	ifs.open(filename);

/*	OutputType::Traits::TokensType fileSize;
	std::getline(ifs, fileSize);
	std::wistringstream ss(fileSize);
	ss >> totalSize;*/

	return ifs.is_open();
}

bool StandartReader::isFileEnd() {

    std::getline(ifs, headLine);
	return ifs.eof();

}

StandartReader::OutputType StandartReader::read() {

	OutputType::Traits::TokensType text;
    std::vector<OutputType::Traits::TokensType> vHeadLine;
    size_t size{};

    Tools::split_regex(vHeadLine, headLine.c_str(), L"|", headLine.size());
    std::wistringstream ss(vHeadLine[HeadLine::Size]);
    ss >> size;

    if(size == 0) {

        size = 1;

    }

    text.assign(size - 1, L' ');
    ifs.read(&text[0], size);

    return Output{ std::make_tuple(vHeadLine[HeadLine::Name].substr(nameOffset), vHeadLine[HeadLine::Url].substr(urlOffset), text) };
}

}
