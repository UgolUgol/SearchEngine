#include <iostream>
#include <boost/algorithm/string/regex.hpp>
#include <vector>
#include <fstream>
#include <streambuf>
#include <locale>
#include <codecvt>
#include <future>
#include <cmath>
#include <chrono>
#include <tools.h>
#include <numeric>

using namespace Tools;
const wchar_t* breakset = L"!#$%&()*+,./:;“„<=>?@[]^_{|}~»«\"\\—\n\t ";

bool isArticle(const std::wstring& token) {

	return token == L"the" || token == L"an";
}


unsigned calculateSize(Tokens& tokens) {

	return tokens.size() + std::accumulate(tokens.cbegin(), tokens.cend(), 0,
	 					   [](unsigned sum, const auto& token) { return sum + token.size(); });
}

Tokens tokenize(const std::wstring& text, const size_t& endline) {

	Tokens tokens;
	split_regex(tokens, text.c_str() + endline + 1, breakset, text.size() - endline - 1);

	auto it = std::remove_if(tokens.begin(), tokens.end(), [](const auto& token){
		return token.size() <= 1 || isArticle(token);
	});
	tokens.erase(it, tokens.end());

	return tokens;

}

void createBigram(Tokens& bigrams,
				  Tokens::const_iterator lborder,
				  Tokens::const_iterator rborder,
				  size_t bucketNum,
				  size_t batchSize) {

	size_t idx = bucketNum * (batchSize - 1);
	for(auto token = lborder; token != rborder; token++) {

		auto nextToken = std::next(token);
		bigrams[idx++] = (*token + L"_" + *nextToken);
		
	}

}

void findBigrams(Tokens& tokens) {

	size_t tokensCount = tokens.size() - 1;
	if(tokensCount <= 1) {
		return;
	}

	std::vector<std::thread> ths;
	Tokens bigrams(tokensCount - 1);

	size_t bucketNum = 0;
	size_t batchSize = tokensCount;
	for(auto token = tokens.cbegin(); token != tokens.cend() - 1; ++bucketNum) {

		auto rborder = std::min(batchSize - 1, static_cast<size_t>(std::distance(token, tokens.cend() - 1)) - 1);
		if(!rborder) {
			break;
		}

		ths.emplace_back(createBigram, std::ref(bigrams), token, std::next(token, rborder), bucketNum, batchSize);
		std::advance(token, rborder);

	}

	for(auto& th : ths) {
		th.join();
	}


	std::move(bigrams.begin(), bigrams.end(), std::back_inserter(tokens));
}

int main(){

	std::wcout.sync_with_stdio(false);
  	std::setlocale(LC_ALL, "ru_RU.utf8");

	std::wstring texts;
	std::vector<std::wstring> splittedTexts;


	texts = readData("res1_url");
	boost::split_regex(splittedTexts, texts, boost::wregex(L"\nWIKIPEDIA_ARTICLE_END\n"));

	ArticleBox articles;
	auto artsCount = splittedTexts.size() - 1;
	articles.names.resize(artsCount);
	articles.urls.resize(artsCount);
	articles.tokens.resize(artsCount);
	std::vector<std::future<void>> futs;

	const auto namePadding = 26;
	const auto urlPadding = 13;
	size_t idx = 0;
	for(auto text = splittedTexts.cbegin(); text != splittedTexts.cend() - 1; ++text, ++idx) {

		const auto delim = (*text).find(L" |WIKI_URL:");
		const auto endline = (*text).find(L"\n");

		auto articleName = (*text).substr(namePadding, delim - namePadding);
		auto url = (*text).substr(delim + urlPadding, endline - namePadding - urlPadding - articleName.size());

		articles.names[idx] = std::move(articleName);
		articles.urls[idx] = std::move(url);
		articles.tokens[idx] = tokenize(*text, endline);
		articles.tokens[idx].emplace_back(L"@DUMMY");
		futs.push_back(std::async(std::launch::async, findBigrams, std::ref(articles.tokens[idx])));
	} 

	for(auto& fut : futs) {
		fut.wait();
	}

	std::wofstream output("tokens");
	output.imbue(std::locale("ru_RU.utf8"));
	std::ostream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t>> it(output, L"\n");

	idx = 0;
	for(;idx < splittedTexts.size() - 1; ++idx) {

		it = articles.names[idx] + L"|" + articles.urls[idx] + L"|" + std::to_wstring(calculateSize(articles.tokens[idx]));
		std::transform(articles.tokens[idx].begin(),
		 			   articles.tokens[idx].end(), it, 
		 			   [](auto& token) {
		 			   		std::transform(token.begin(), token.end(), token.begin(), std::towlower);
		 			   		return token;
		 			   });
	}
	output.close();

}
