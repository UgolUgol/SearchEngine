#include <iostream>
#include <locale>
#include <index_maker.h>
#include "reader.h"
#include "input_handler.h"

int main() {
	std::setlocale(LC_ALL, "ru_RU.utf8");

	StandartReader r;
	InputHandler h;
	r.openFile("tokens");
	typename InputHandler::OutputType data;
	while(!r.isFileEnd()) {
		
		InputHandler::OutputTraits::concatenate(data, h.prepareForSort(r.read()));	

	}

}
