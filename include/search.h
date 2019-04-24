#include <vector>
#include <iostream>
#include <string>
#include <search_tree.h>

namespace Traits {


	template<typename T> 
	struct is_string {
		static constexpr bool v = false;
	};

	template<typename CharT> 
	struct is_string<std::basic_string<CharT>> {
		static constexpr bool v = true;
	};

	template<> 
	struct is_string<char*> {
		static constexpr bool value = true;
	};

	template<>
	struct is_string<const char*> {
		static constexpr bool value = true;
	};

	template<typename T> inline constexpr bool is_string_v = is_string<T>::value;
}



namespace Search {

	struct Response {
		std::wstring articleName;
		std::wstring url;
	};


	class RequestHandler {
	public:
		template<typename T> std::vector<Response> search(T&& request);  

	private:
		template<typename T> T makeInverseExpression(T&& request);
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
