#include "input_handler.h"
#include <functional>
#include <tools.h>
#include "reader.h"

namespace InputHandler {

size_t StandartHandler::docId = 1;

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForSort(Input&& input) {

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

		output.data.emplace_back(std::hash<std::wstring>{}(token), 
								 docId,
								 articleName,
								 url,
								 position++);
	}
	++docId;
	return output;
}

template StandartHandler::OutputType StandartHandler::prepareForSort<Reader::Output>(Reader::Output&& input);

}
