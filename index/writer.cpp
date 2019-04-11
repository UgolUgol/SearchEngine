#include "writer.h"
#include "output_handler.h"
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

	auto dict = std::get<Input::Traits::DictFile>(input.data);
	auto coord = std::get<Input::Traits::CoordFile>(input.data);
	dictOfs.write((char*)dict.data(), sizeof(typename Input::Traits::DictType) * dict.size());
	coordOfs.write((char*)coord.data(), sizeof(typename Input::Traits::CoordType) * coord.size());

	return true;
}

template bool StandartWriter
::write<typename OutputHandler::StandartHandler::OutputType>(typename OutputHandler::StandartHandler::OutputType&& input);

}
