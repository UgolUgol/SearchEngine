#include <iostream>
#include <locale>
#include <index_maker.h>
#include "reader.h"

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");

	StandartReader r;
	r.openFile("tokens");
	typename StandartReader::OutputType data;
	while(!r.isFileEnd()) {
		data = r.read();
		std::wcout<<std::get<0>(data)<<std::endl;
	}

}
