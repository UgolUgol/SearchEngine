#include "input_handler.h"
#include <functional>
#include <tools.h>
#include "reader.h"

size_t InputHandler::docId = 1;

template<typename Input>
InputHandler::OutputType InputHandler::prepareForSort(Input&& input) {

	OutputType output;

	std::vector<typename Input::Traits::TokensType> tokens;
	typename Input::Traits::NameType articleName = std::move(std::get<Input::Traits::Name>(input.data));
	typename Input::Traits::UrlType url = std::move(std::get<Input::Traits::Url>(input.data));
	typename Input::Traits::TokensType text = std::move(std::get<Input::Traits::Tokens>(input.data));
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

template InputHandler::OutputType InputHandler::prepareForSort<Reader::Output>(Reader::Output&& input);
