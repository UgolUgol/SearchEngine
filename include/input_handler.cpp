#include "input_handler.h"
#include <functional>
#include <tools.h>
#include "reader.h"
#include <iostream>
namespace InputHandler {

Output::DocId StandartHandler::docId = 1;

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForSort(Input&& input) {

	OutputType output;

	OutputType::PositionType position = 1;
	std::vector<typename Input::Traits::TokensType> tokens;
	typename Input::Traits::NameType articleName = std::move(std::get<Input::Traits::Name>(input.data));
	typename Input::Traits::UrlType url = std::move(std::get<Input::Traits::Url>(input.data));
	typename Input::Traits::TokensType text = std::move(std::get<Input::Traits::Tokens>(input.data));

	Tools::split_regex(tokens, text.c_str(), Input::separator, text.size());
	for(auto& token : tokens) {
		
		if(token == L"@dummy") {
			position = 1;
			continue;
		}

		output.data.emplace_back(std::hash<typename Input::Traits::TokensType>{}(token), 
								 docId,
								 articleName,
								 url,
								 position++);
	}
	if(!output.data.empty()) {
		++docId;
	}

	return output;
}

template StandartHandler::OutputType StandartHandler::prepareForSort<Reader::Output>(Reader::Output&& input);

}
