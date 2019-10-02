#include "writer.h"
#include "output_handler.h"
#include <iostream>
namespace Writer {

StandartWriter::StandartWriter() {
	invCoordOfs.imbue(std::locale("ru_RU.utf8"));
}


StandartWriter::~StandartWriter() {
	dictOfs.close();
	coordOfs.close();
	invCoordOfs.close();
}


bool StandartWriter::openFiles(const std::string& dictFile, const std::string& coordFile, const std::string& invCoordFile) {

	dictOfs.open(directory + dictFile + ".bin", std::ios::binary);
	coordOfs.open(directory + coordFile + ".bin", std::ios::binary);
	invCoordOfs.open(directory + invCoordFile + ".bin", std::ios::binary);

	return dictOfs.is_open() && coordOfs.is_open() && invCoordOfs.is_open();
}

template<typename Input>
bool StandartWriter::write(Input&& input) {

    std::wcout << "Start writting ... \n";

	auto dict = std::get<Input::Traits::DictFile>(input.data);
	auto coord = std::get<Input::Traits::CoordFile>(input.data);
	auto invCoord = std::get<Input::Traits::InvCoordFile>(input.data);

	dictOfs.write(reinterpret_cast<char*>(dict.data()),
	 			  sizeof(typename Input::Traits::DictType) * dict.size());
	coordOfs.write(reinterpret_cast<char*>(coord.data()),
	               sizeof(typename Input::Traits::CoordType) * coord.size());

	invCoordOfs.write(reinterpret_cast<char*>(&invCoord.head),
	                  sizeof(typename Input::Traits::InvCoordType::HeadType));
	invCoordOfs.write(reinterpret_cast<char*>(invCoord.body.data()),
	                  sizeof(typename Input::Traits::InvCoordType::BodyType::value_type) * invCoord.body.size());
	invCoordOfs.write(reinterpret_cast<char*>(invCoord.bottom.data()),
				      sizeof(typename Input::Traits::InvCoordType::BottomType::value_type) * invCoord.bottom.size());
	return true;
}

template bool StandartWriter
::write<typename OutputHandler::StandartHandler::OutputType>(typename OutputHandler::StandartHandler::OutputType&& input);


template<>
void write<std::wstring>(const char* filename, std::vector<std::wstring>&& data)
{

    std::ofstream ofs;
    ofs.open(filename, std::ios::binary|std::ios::app);

    for(auto&& str : data) {

        ofs.write(reinterpret_cast<char*>(str.data()), str.size() * sizeof(wchar_t));

    }

    ofs.close();


}


}
