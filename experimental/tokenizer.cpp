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
#include <reader.h>


using namespace Tools;
const wchar_t* breakset = L"!#$%&()*+,./:;“„<=>?@[]^_{|}~»«\"\\—\n\t ";

bool isArticle(const std::wstring& token) {

    return token == L"the" || token == L"an" || token == L"a";
}


unsigned calculateSize(const Tokens& tokens) {

    return tokens.size() + std::accumulate(tokens.cbegin(), tokens.cend(), 0,
                                           [](unsigned sum, const auto& token) { return sum + token.size(); });
}

Tokens tokenize(const std::wstring& text, const size_t& endline) {

    Tokens tokens;
    split_regex(tokens, text.c_str() + endline + 1, breakset, text.size() - endline - 1);

    auto it = std::remove_if(tokens.begin(), tokens.end(), [](const auto& token){
        return isArticle(token);
    });
    tokens.erase(it, tokens.end());

    return tokens;

}


int main() {

    //std::ios_base::sync_with_stdio(false);
    std::setlocale(LC_ALL, "ru_RU.utf8");

    Reader::StandartReader reader;
    reader.openFile("res1_url");

    std::wofstream output("tokens");
    output.imbue(std::locale("ru_RU.utf8"));
    std::ostream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t>> it(output, L"\n");

    while(!reader.isFileEnd()) {

        auto [name, url, text] = reader.read().data;
        const auto endline = (text).find(L"\n");

        auto tokens = tokenize(text, endline);
        auto articleSize = std::to_wstring(calculateSize(tokens));

        it = name + L"|" + url + L"|" + articleSize;
        std::transform(std::begin(tokens), std::end(tokens), it,
                       [](auto& token) {
                           std::transform(token.begin(), token.end(), token.begin(), std::towlower);
                           return token;
                       });

    }



    return 0;
}
