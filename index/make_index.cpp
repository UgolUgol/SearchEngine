#include <iostream>
#include <locale>
#include <index_maker.h>
#include <reader.h>
#include <input_handler.h>
#include <sorter.h>
#include <output_handler.h>
#include <writer.h>


int main() {
	Indexer<Reader::StandartReader, InputHandler::StandartHandler,
	 		Sorter::QuickSorter, OutputHandler::StandartHandler,
 		    Writer::StandartWriter> indexer("tokens", "dict", "coord", "invCoord") ;
 	indexer.make();

}
