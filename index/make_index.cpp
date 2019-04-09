#include <iostream>
#include <locale>
#include <index_maker.h>
#include "reader.h"
#include "input_handler.h"
#include "sorter.h"

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");

	Reader::StandartReader r;
	InputHandler h;
	Sorter s;
	r.openFile("tokens");
	typename InputHandler::OutputType data;
	while(!r.isFileEnd()) {
		
		InputHandler::OutputTraits::concatenate(data, h.prepareForSort(r.read()));	

	}

	//typename Sorter::OutputType sortData = s.sort(data);

}
