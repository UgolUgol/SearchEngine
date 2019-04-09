#include <iostream>
#include <locale>
#include <index_maker.h>
#include "reader.h"
#include "input_handler.h"
#include "sorter.h"

class Coord {
	int val;
public:
	int get() { return val; }
	operator int&() { return val; }
};

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");
	Reader::StandartReader r;
	InputHandler::StandartHandler h;
	Sorter s;
	r.openFile("tokens");
	typename InputHandler::StandartHandler::OutputType data;
	while(!r.isFileEnd()) {
		
		InputHandler::StandartHandler::OutputType::Traits::concatenate(data, h.prepareForSort(r.read()));	

	}

	typename Sorter::OutputType sortData = s.sort(data);

}
