#pragma once
#include <string_view>
#include <set>
#include <deque>
#include <vector>
#include <iostream>


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

    std::basic_string_view<T> word;
    std::basic_string_view<T> RV;
    std::basic_string_view<T> R1;
    std::basic_string_view<T> R2;

    Morphology morph;
public:

    Stemmer() = delete;
    Stemmer(const Stemmer&) = delete;
    Stemmer(Stemmer&&) = delete;
    explicit Stemmer(const std::basic_string_view<T> word);

    Stemmer& operator=(Stemmer&&) = delete;
    Stemmer& operator=(const Stemmer&) = delete;


    std::string normalize();
};
