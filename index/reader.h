#pragma once

#include <string>
#include <fstream>
#include <tuple>

enum HeadLine: size_t;

class StandartReader {
	std::wifstream ifs;
public:
	using OutputType = std::tuple<std::wstring, std::wstring, std::wstring>;

	StandartReader();
	~StandartReader();

	bool openFile(const std::string& filename);
	bool isFileEnd();
	OutputType read();

};