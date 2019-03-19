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


using Tokens = std::vector<std::wstring>;
using ArticleBox = std::tuple<std::wstring, std::wstring, Tokens>;
const wchar_t* breakset = L"!#$%&()*+,./:;<=>?@[]^_{|}~»«\"\\—\n ";

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


void split_regex(Tokens& dest, const wchar_t* src, const wchar_t* breakset, size_t srcSize) {

	const wchar_t* end = src + srcSize;
	while(src != end) {
		auto str = std::wcspbrk(src, breakset);
		if(!str) {
			dest.emplace_back(src);
			break;
		}

		size_t diff = str - src;
		if(diff) {
			dest.emplace_back(src, diff);
		}
		src = str + 1;
	}
}


bool isArticle(const std::wstring& token) {

	return token == L"the";
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
		bigrams[idx++] = (*token + L" " + *nextToken);
		
	}

}

void findBigrams(Tokens& tokens) {

	size_t tokensCount = tokens.size();
	if(tokensCount <= 1) {
		return;
	}
	size_t batchSize = tokensCount;

	Tokens bigrams(tokensCount - 1);
	std::vector<std::thread> ths;

	size_t bucketNum = 0;
	for(auto token = tokens.cbegin(); token != tokens.cend(); ++bucketNum) {

		auto rborder = std::min(batchSize - 1, static_cast<size_t>(std::distance(token, tokens.cend())) - 1);
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
  	std::wcout.imbue(std::locale("ru_RU.utf8"));

	std::wstring texts;
	std::vector<std::wstring> splittedTexts;

	texts = readData("res1_url");
	boost::split_regex(splittedTexts, texts, boost::wregex(L"\nWIKIPEDIA_ARTICLE_END\n"));

	std::vector<ArticleBox> articlesTokens(splittedTexts.size() - 1);
	std::vector<std::future<void>> futs;

	const auto namePadding = 26;
	const auto urlPadding = 13;
	size_t idx = 0;

	for(auto text = splittedTexts.cbegin(); text != splittedTexts.cend() - 1; text++) {

		const auto delim = (*text).find(L" |WIKI_URL:");
		const auto endline = (*text).find(L"\n");

		const auto articleName = (*text).substr(namePadding, delim - namePadding);
		const auto url = (*text).substr(delim + urlPadding, endline - namePadding - urlPadding - articleName.size());

		articlesTokens[idx] = std::make_tuple(articleName, url, tokenize(*text, endline));
		futs.push_back(std::async(std::launch::async, findBigrams, std::ref(std::get<2>(articlesTokens[idx++]))));
	} 

	for(auto& fut : futs) {
		fut.wait();
	}

	std::wofstream output("tokens");
	output.imbue(std::locale("ru_RU.utf8"));
	std::ostream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t>> it(output, L"\n");

	for(const auto& article : articlesTokens) {
		it = L"WIKIPEDIA_ARTICLE_BEGIN: " + std::get<0>(article) + L" | WIKI_URL: " + std::get<1>(article);
		auto tokens = std::get<2>(article);
		std::copy(tokens.begin(), tokens.end(), it);
		it = L"WIKIPEDIA_ARTICLE_END";
	}
	output.close();

}
