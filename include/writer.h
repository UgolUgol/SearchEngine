#pragma once 

#include <vector>
#include <fstream>
#include <string>
#include <iostream>


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


template<typename T>
void write(const char* filename, std::vector<T>&& data)
{

    std::ofstream ofs;
    ofs.open(filename, std::ios::binary|std::ios::app);

    ofs.write(reinterpret_cast<char*>(data.data()), data.size() * sizeof(T));
    ofs.close();

}

template<typename T>
void write(const char* filename, T&& value)
{

    std::ofstream ofs;
    ofs.open(filename, std::ios::binary|std::ios::app);
    ofs.write(reinterpret_cast<char*>(&value), sizeof(T));
    ofs.close();

}



}
