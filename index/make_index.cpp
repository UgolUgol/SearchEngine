#include <iostream>
#include <locale>
#include <index_maker.h>
#include "reader.h"
#include "input_handler.h"
#include "sorter.h"
#include "output_handler.h"
#include "writer.h"
#include <algorithm>
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
	Sorter::QuickSorter s;
	OutputHandler::StandartHandler oh;
	Writer::StandartWriter w;
	r.openFile("tokens");
	typename InputHandler::StandartHandler::OutputType data;
	while(!r.isFileEnd()) {
		
		InputHandler::StandartHandler::OutputType::Traits::concatenate(data, h.prepareForSort(r.read()));	

	}

	w.openFiles("dict", "coord", "invCoord");
	w.write(oh.prepareForWrite(s.sort(data)));

}
