#include <iostream>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <regex>
#include <vector>
#include <fstream>
#include <streambuf>
#include <locale>
#include <codecvt>
#include <future>

using Tokens = std::vector<std::wstring>;
using ArticleBox = std::tuple<std::wstring, std::wstring, Tokens>;

std::wstring readData(std::string filename) {

	std::wifstream ifs(filename);
	ifs.imbue(std::locale("ru_RU.utf8"));

	ifs.seekg(0, std::ios::end);
	size_t size = ifs.tellg();
	std::wstring texts(size, L' ');
	ifs.seekg(0, std::ios::beg);
	
	ifs.read(&texts[0], size);
	ifs.close();

	return texts;
}



bool isArticle(const std::wstring& token) {

	return token == L"the";
}


Tokens tokenize(const std::wstring& text, const size_t& endline) {

	Tokens tokens;
	boost::split_regex(tokens, text.c_str() + endline, boost::wregex(L"[[:punct:]\\s»«]+", boost::wregex::extended ));

	auto it = std::remove_if(tokens.begin(), tokens.end(), [](const auto& token){
		return token.size() == 1 || isArticle(token);
	});
	tokens.erase(it, tokens.end());

	return tokens;

}

void findBigrams(Tokens& tokens) {

	Tokens bigrams(tokens.size() - 1);

	size_t idx = 0;
	for(auto token = tokens.cbegin(); token != tokens.cend() - 1; token++) {
		auto nextToken = std::next(token);
		bigrams[idx++] = (*token + L" " + *nextToken);
	}

	std::move(bigrams.begin(), bigrams.end(), std::back_inserter(tokens));
}


int main(){

	std::wcout.sync_with_stdio(false);
  	std::wcout.imbue(std::locale("ru_RU.utf8"));

	std::wstring texts;
	std::vector<std::wstring> splittedTexts;

	texts = readData("res1_url");
	boost::split_regex(splittedTexts, texts, boost::wregex(L"\nWIKIPEDIA_ARTICLE_END\n"));

	std::vector<ArticleBox> articlesTokens(splittedTexts.size() - 1);
	std::vector<std::thread> ths(splittedTexts.size() - 1);
	

	const auto namePadding = 26;
	const auto urlPadding = 13;
	size_t idx = 0;

	for(auto text = splittedTexts.cbegin(); text != splittedTexts.cend() - 1; text++) {

		const auto delim = (*text).find(L" |WIKI_URL:");
		const auto endline = (*text).find(L"\n");

		const auto articleName = (*text).substr(namePadding, delim - namePadding);
		const auto url = (*text).substr(delim + urlPadding, endline - namePadding - urlPadding - articleName.size());

		articlesTokens[idx] = std::make_tuple(articleName, url, tokenize(*text, endline));
		ths[idx++] = std::thread(findBigrams, std::ref(std::get<2>(articlesTokens[idx])));

	} 


	for(auto& th : ths) { 
		th.join();
	}

	for(const auto& article : articlesTokens) {

		std::wcout<<L"WIKIPEDIA_ARTICLE_BEGIN: "<<std::get<0>(article)<<L" | WIKI_URL: "<<std::get<1>(article)<<std::endl;
		for(const auto& token : std::get<2>(article)) {
			std::wcout<<token<<std::endl;
		}
	}




	
}