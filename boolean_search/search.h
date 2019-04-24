#include <vector>
#include <iostream>
#include <string>

namespace Traits {


	template<typename T> 
	struct is_string {
		static constexpr bool v = false;
	};

	template<typename CharT> 
	struct is_string<std::basic_string<CharT>, std::char_traits<CharT>> {
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
		template<typename T> std::vector<Repsonse> search(T&& request);  

	private:
		template<typename T> T makeInverseExpression(T&& request);
		SearchTree tree;
	};



	template<typename T>
	std::vector<Response> RequestHandler::search(T&& request) {

		using ClearType = std::decay_t<T>;
		if constexpr(Traits::is_string_v<ClearType>) {

			auto inverseExpression = makeInverseExpression(std::forward<T>(request));
			tree.build(inverseExpression);

		} 

		return tree.get();
	}


	template<typename T>
	T RequestHandler::makeInverseExpression(T&& request) {



		return std::forward<T>(request);
	}

};
