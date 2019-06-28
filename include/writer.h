#pragma once 

#include <vector>
#include <fstream>
#include <string>


namespace Writer {


class StandartWriter {
	std::ofstream dictOfs;
	std::ofstream coordOfs;
	std::ofstream invCoordOfs;

	std::string directory{"../index_files/"};
public:
	StandartWriter();
	~StandartWriter();

	bool openFiles(const std::string& dictFile, const std::string& coordFile, const std::string& invCoordOfs);
	template<typename Input> bool write(Input&& write);

};



}
