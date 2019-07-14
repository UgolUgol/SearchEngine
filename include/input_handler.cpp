#include "input_handler.h"
#include <functional>
#include <tools.h>
#include "reader.h"
#include <iostream>
#include <stemmer.h>


namespace InputHandler {

Output::DocId StandartHandler::docId = 1;

template<typename Input>
StandartHandler::OutputType StandartHandler::prepareForSort(Input&& input) {

    using NameType = typename Input::Traits::NameType;
    using UrlType = typename Input::Traits::UrlType;
    using TokensType = typename Input::Traits::TokensType;

	OutputType output;

	OutputType::PositionType position = 1;
	std::vector<TokensType> tokens;

	NameType articleName = std::move(std::get<Input::Traits::Name>(input.data));
	UrlType url = std::move(std::get<Input::Traits::Url>(input.data));
	TokensType text = std::move(std::get<Input::Traits::Tokens>(input.data));


	Tools::split_regex(tokens, text.c_str(), Input::separator, text.size());
	for(auto& token : tokens) {
		
		if(token == L"@dummy") {
			break;
			position = 1;
			continue;
		}

        Stemmer<typename TokensType::value_type> st(token);
		output.data.emplace_back(std::hash<TokensType>{}(static_cast<TokensType>(st.normalize())),
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
