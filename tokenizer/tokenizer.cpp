#include <iostream>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <regex>
#include <vector>
#include <fstream>
#include <streambuf>
#include <locale>
#include <codecvt>


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


void tokenize(const std::wstring& text, const size_t& endline) {

	std::vector<std::wstring> tokens;
	boost::split_regex(tokens, text.c_str() + endline, boost::wregex(L"[[:punct:]\\s»«]+", boost::wregex::extended ));


	std::copy_if(tokens.cbegin(), tokens.cend(), 
				 std::ostream_iterator<std::wstring, wchar_t>(std::wcout, L"\n"),
	 		  	 [](const std::wstring& token) { return token.size() > 1 && ! isArticle(token); });
}


int main(){

	std::wcout.sync_with_stdio(false);
  	std::wcout.imbue(std::locale("ru_RU.utf8"));

	std::wstring texts;
	std::vector<std::wstring> splittedTexts;
	texts = readData("res1_url");
	boost::split_regex(splittedTexts, texts, boost::wregex(L"\nWIKIPEDIA_ARTICLE_END\n"));

	const auto namePadding = 26;
	const auto urlPadding = 13;
	std::for_each(splittedTexts.cbegin(), splittedTexts.cend() - 1,
	 			  [&namePadding, &urlPadding](const auto& text) {

						const auto delim = text.find(L" |WIKI_URL:");
						const auto endline = text.find(L"\n");

						const auto articleName = text.substr(namePadding, delim - namePadding);
						const auto url = text.substr(delim + urlPadding, endline - namePadding - urlPadding - articleName.size());
	 			   		
	 			   		std::wcout<<L"WIKIPEDIA_ARTICLE_BEGIN: "<< articleName << "|" << url << std::endl;
	 			   		tokenize(text, endline);
	 			   		std::wcout<<L"WIKIPEDIA_ARTICLE_END"<<std::endl;
	 			   });
	


	
}