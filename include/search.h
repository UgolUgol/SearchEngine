#include <vector>
#include <iostream>
#include <string>
#include <search_tree.h>

namespace Search {

	struct Response {
		std::wstring articleName;
		std::wstring url;
	};


	class RequestHandler {
	public:
		template<typename T> std::vector<Response> search(T&& request);  

	private:
		std::vector<Response> extractFromTree();

		SearchTree tree;
	};



	template<typename T>
	std::vector<Response> RequestHandler::search(T&& request) {

		using ClearType = std::decay_t<T>;
		if constexpr(Traits::is_string_v<ClearType>) {

			tree.build(std::forward<T>(request));

		} 

		return extractFromTree();
	}


	std::vector<Response> RequestHandler::extractFromTree() {
		
		return {};
	}

};
