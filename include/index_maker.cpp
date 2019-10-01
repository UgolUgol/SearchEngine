#include "index_maker.h"
#include "reader.h"
#include "input_handler.h"
#include "sorter.h"
#include "output_handler.h"
#include "writer.h"
#include <partition_file.h>
#include <iostream>

template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
Indexer<Reader, InputHandler, Sorter, OutputHandler, Writer>
::Indexer(const std::string& tokensFile, 
		  const std::string& dictFile,
		  const std::string& coordFile,
		  const std::string& invCoordFile) :
		  tokensFile(tokensFile),
		  dictFile(dictFile),
		  coordFile(coordFile),
		  invCoordFile(invCoordFile) { }



template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
void Indexer<Reader, InputHandler, Sorter, OutputHandler, Writer>
::cacheOnDisk(typename InputHandler::OutputType&& output, const char *fname) const
{
    std::wcout << "Write part\n";

    std::ofstream ofs;
    ofs.open(fname, std::ios::binary|std::ios::app);

    ofs.write(reinterpret_cast<char*>(output.data.data()), output.data.size() * sizeof(typename InputHandler::OutputType::value_type));
    ofs.close();

}

template<typename Reader, typename InputHandler, typename Sorter, typename OutputHandler, typename Writer>
bool Indexer<Reader, InputHandler, Sorter, OutputHandler, Writer>::make() {


    Reader::openFile(tokensFile);
    Writer::openFiles(dictFile, coordFile, invCoordFile);

    bool readAll{Reader::isFileEnd()};
    std::setlocale(LC_ALL, "ru_RU.utf8");

    while(!readAll) {

        typename InputHandler::OutputType data;
        unsigned articlesCount{1000};

        while((articlesCount--) > 0) {

            InputHandler::OutputType::Traits::concatenate(data, InputHandler::prepareForSort(Reader::read()));

            readAll = Reader::isFileEnd();
            if(readAll) {

                break;

            }

        }

        if(!readAll) {

            cacheOnDisk(std::move(data), "cached");

        }
        break;
    }

    DiskCachedData<typename InputHandler::OutputType::value_type> data("cached");
    return Writer::write(OutputHandler::prepareForWrite(Sorter::sort(data)));

	//return Writer::write(OutputHandler::prepareForWrite(Sorter::sort(data)));
}

template class Indexer<Reader::StandartReader,
 					   InputHandler::StandartHandler,
 					   Sorter::ExternalSorter,
 					   OutputHandler::StandartHandler,
 					   Writer::StandartWriter>;
