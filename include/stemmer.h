#pragma once
#include <string_view>
#include <set>
#include <deque>
#include <vector>
#include <iostream>
#include <algorithm>


template<typename T> using SuffixContainer = std::vector<std::basic_string_view<T>>;
template<typename T> using PreSuffixContainer = std::initializer_list<T>;

namespace algorithms {

    template<typename T>
    bool hasSuffix(std::basic_string_view<T> str, std::basic_string_view<T> suffix)
    {

        if(suffix.length() > str.length()) {

            return false;

        }

        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;

    }

    template<typename T>
    auto findComparableSuffix(const std::basic_string_view<T> word,
                              const SuffixContainer<T>& suffixes,
                              const PreSuffixContainer <T>& preSuffix = {})
    {

        auto endCompare = [&word, &preSuffix](const auto& ending) {

            auto suffixCompare = hasSuffix(word, ending);
            if(!suffixCompare) {

                return false;

            }

            auto preSuffixListEmpty = (std::begin(preSuffix) == std::end(preSuffix));
            auto n = word.length() - ending.length() - 1;

            return preSuffixListEmpty ||
                   ( n >= 0 && std::find(std::begin(preSuffix), std::end(preSuffix), word[n]) != std::end(preSuffix));

        };

        return std::find_if(std::cbegin(suffixes), std::cend(suffixes), endCompare);


    }
}



struct Morphology {

    static const std::vector<wchar_t> vowels;

    struct Endings {

        static const std::vector<std::wstring_view> perfectiveGerund1;
        static const std::vector<std::wstring_view> perfectiveGerund2;
        static const std::vector<std::wstring_view> adjective;
        static const std::vector<std::wstring_view> participle1;
        static const std::vector<std::wstring_view> participle2;
        static const std::vector<std::wstring_view> reflexive;
        static const std::vector<std::wstring_view> verb1;
        static const std::vector<std::wstring_view> verb2;
        static const std::vector<std::wstring_view> noun;
        static const std::vector<std::wstring_view> superlative;
        static const std::vector<std::wstring_view> derivational;

    } endings;

};


template<typename T>
class Stemmer {
private:
    std::basic_string_view<T> word;
    std::basic_string_view<T> result;

    std::size_t RV;
    std::size_t R1;
    std::size_t R2;

    Morphology morph;

    std::size_t findRV(std::basic_string_view<T> word);
    std::size_t findR(std::basic_string_view<T> word);

    void firstStep();
    void secondStep();
    void thirdStep();
    void fourthStep();
    void recalculatePositions();

public:

    Stemmer() = delete;
    Stemmer(const Stemmer&) = delete;
    Stemmer(Stemmer&&) = delete;
    explicit Stemmer(const std::basic_string_view<T> word);

    Stemmer& operator=(Stemmer&&) = delete;
    Stemmer& operator=(const Stemmer&) = delete;


    std::basic_string_view<T> normalize();
};
