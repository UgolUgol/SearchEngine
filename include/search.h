#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <search_tree.h>

namespace Search {

	class RequestHandler {
	public:
		RequestHandler();
		~RequestHandler() = default;
		template<typename T> std::vector<SearchResultBlock> search(T&& request);  
	private:
		DirectIndex<DefaultIndex> index;
	};


	RequestHandler::RequestHandler() : index("../index_files/invCoord.bin") { }

	template<typename T>
	std::vector<SearchResultBlock> RequestHandler::search(T&& request) {

		using ClearType = std::decay_t<T>;
		SearchTree tree;
		if constexpr(Traits::is_string_v<ClearType>) {

			tree.build(std::forward<T>(request));

		} 

		auto docIds = tree.extractResults();
		return algorithms::formSearchResult(docIds, index);
	}


};
