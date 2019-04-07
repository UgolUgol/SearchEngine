#include "input_handler.h"
#include <functional>
#include <tools.h>

enum Input : size_t { ArticleName=0, Url, Tokens };

size_t InputHandler::docId = 1;

template<typename InputType>
typename InputHandler::OutputType InputHandler
::prepareForSort(InputType&& input) {

	OutputType output;

	std::vector<std::wstring> tokens;
	std::wstring articleName = std::move(std::get<ArticleName>(input));
	std::wstring url = std::move(std::get<Url>(input));
	std::wstring text = std::move(std::get<Tokens>(input));
	size_t position = 1;

	Tools::split_regex(tokens, text.c_str(), L"\n", text.size());
	for(auto& token : tokens) {
		
		if(token == L"@dummy") {
			position = 1;
			continue;
		}

		output.emplace_back(std::hash<std::wstring>{}(token), 
							docId,
							articleName,
							url,
							position++);
	}
	++docId;
	return output;
}

typename InputHandler::OutputType InputHandler
::operator+(const typename InputHandler::OutputType& rhs) {


}

using InputType = std::tuple<std::wstring, std::wstring, std::wstring>;
template typename InputHandler::OutputType InputHandler
::prepareForSort<InputType>(InputType&& input);