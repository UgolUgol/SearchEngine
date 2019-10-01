#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <tuple>
#include "input_handler.h"
#include <partition_file.h>

namespace Sorter {

	using HashType = size_t;
	using DocId = size_t;
	using Name = std::wstring;
	using URL = std::wstring;
	using PositionType = size_t;


	class QuickSorter {
	public:
		using OutputType = InputHandler::Output;
		template<typename Input> OutputType sort(Input& input);
	};

	class ExternalSorter {
	public:
        template<typename T> using OutputType = DiskCachedData<T>;
		template<typename NodeType> OutputType <NodeType> sort(OutputType<NodeType>& input);
	};


};
