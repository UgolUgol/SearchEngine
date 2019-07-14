#include "stemmer.h"
#include <algorithm>

const std::vector<wchar_t>
Morphology::vowels { L'а', L'е', L'и', L'о', L'у', L'ы', L'э', L'ю', L'я'};

const std::vector<std::wstring_view>
Morphology::Endings::perfectiveGerund1 ({L"вшись", L"вши", L"в"});

const std::vector<std::wstring_view>
Morphology::Endings::perfectiveGerund2 ({L"ившись", L"ывшись", L"ивши", L"ывши", L"ыв", L"ив"});

const std::vector<std::wstring_view>
Morphology::Endings::adjective ({
        L"его", L"ого", L"ему", L"ому", L"ими", L"ыми", L"ее", L"ие",
        L"ые", L"ое",  L"ей", L"ий", L"ый", L"ой", L"ем", L"им", L"ым", L"ом",
        L"их", L"ых", L"ую", L"юю", L"ая", L"яя", L"ою", L"ею"
});

const std::vector<std::wstring_view>
Morphology::Endings::participle1 ({L"ем", L"нн", L"вш", L"ющ", L"щ"});

const std::vector<std::wstring_view>
Morphology::Endings::participle2 ({L"ивш", L"ывш", L"ующ"});

const std::vector<std::wstring_view>
Morphology::Endings::reflexive ({L"ся", L"сь"});

const std::vector<std::wstring_view>
Morphology::Endings::verb1 ({
        L"йте", L"ете", L"ешь", L"нно", L"ла", L"на", L"ли", L"ем", L"ло", L"но",  L"ет", L"ют", L"ны", L"ть", L"й",
        L"л", L"н"
});

const std::vector<std::wstring_view>
Morphology::Endings::verb2 ({
        L"ейте", L"уйте", L"ила", L"ыла", L"ена", L"ите", L"или", L"ыли", L"ило", L"ыло",  L"ует",
        L"уют", L"ены", L"ить", L"ыть", L"ишь", L"ено", L"ей", L"уй", L"ил", L"ыл", L"им", L"ым",
        L"ен", L"ят", L"ит", L"ыт", L"ую", L"ю"
});


const std::vector<std::wstring_view>
Morphology::Endings::noun ({
        L"иями", L"ями", L"ами", L"ией", L"иям", L"ием", L"иях", L"ев", L"ов", L"ие", L"ье", L"еи",
        L"ии", L"ей", L"ой", L"ий", L"ям", L"ем", L"ам", L"ом", L"ах", L"ам", L"ях", L"ию", L"ью", L"ия",
        L"ья", L"а", L"е", L"и", L"о", L"й", L"у", L"ы", L"ь", L"ю", L"я"
});



const std::vector<std::wstring_view>
Morphology::Endings::superlative ({ L"ейшe", L"ейш" });

const std::vector<std::wstring_view>
Morphology::Endings::derivational ({ L"ость", L"ост" });


template<typename T>
Stemmer<T>::Stemmer(const std::basic_string_view<T> word) : word(word)
{

    RV = findRV(word);
    R1 = findR(word);
    R2 = R1 + findR(word.substr(R1));

}


template<typename T>
std::size_t Stemmer<T>::findRV(std::basic_string_view<T> word)
{

    auto isVowel = [&vowels=morph.vowels](auto& letter) {
        return std::find(std::cbegin(vowels), std::cend(vowels), letter) != std::cend(vowels);
    };

    auto firstVowel = std::find_if(std::cbegin(word), std::cend(word), isVowel);
    std::size_t position = std::distance(std::cbegin(word), firstVowel);

    return position < word.length() ? position + 1 : word.length();
}


template<typename T>
std::size_t Stemmer<T>::findR(std::basic_string_view<T> word)
{

    auto isVowel = [&vowels=morph.vowels](auto& letter) {
        return std::find(std::cbegin(vowels), std::cend(vowels), letter) != std::cend(vowels);
    };

    auto vowelConsonantFind = [&vowels=morph.vowels, &isVowel](const auto& a, const auto& b) {

        return isVowel(a) && !isVowel(b);

    };
    auto firstVowelConsonant = std::adjacent_find(std::cbegin(word), std::cend(word), vowelConsonantFind);


    std::size_t position = std::distance(std::cbegin(word), firstVowelConsonant);
    return position < word.length() ? position + 2 :  word.length();

}


template<typename T>
std::basic_string_view<T> Stemmer<T>::normalize()
{

    result = word;

    firstStep();
    secondStep();
    thirdStep();
    fourthStep();

    return result;

}

template<typename T>
void Stemmer<T>::recalculatePositions()
{

    RV = std::min(RV, result.length());
    R1 = std::min(R1, result.length());
    R2 = std::min(R2, result.length());

}

template<typename T>
void Stemmer<T>::firstStep()
{

    auto suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.perfectiveGerund1, {L'а', L'я'});
    if(suffix != morph.endings.perfectiveGerund1.cend()) {

        result = result.substr(0, result.length() - (*suffix).length() - 1);
        recalculatePositions();
        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.perfectiveGerund2);
    if(suffix != morph.endings.perfectiveGerund2.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();
        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.reflexive);
    if(suffix != morph.endings.reflexive.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();
        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.adjective);
    if(suffix != morph.endings.adjective.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();

        auto partSuffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.participle1, {L'а', L'я'});
        if(partSuffix != morph.endings.participle1.cend()) {

            result = result.substr(0, result.length() - (*partSuffix).length());
            recalculatePositions();
            return;

        }

        partSuffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.participle2);
        if(partSuffix != morph.endings.participle1.cend()) {

            result = result.substr(0, result.length() - (*partSuffix).length());
            recalculatePositions();
            return;

        }

        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.verb1, {L'а', L'я'});
    if(suffix != morph.endings.verb1.cend()) {

        result = result.substr(0, result.length() - (*suffix).length() - 1);
        recalculatePositions();
        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.verb2);
    if(suffix != morph.endings.verb2.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();
        return;

    }

    suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.noun);
    if(suffix != morph.endings.noun.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();
        return;

    }

}

template<typename T>
void Stemmer<T>::secondStep()
{

    if(algorithms::hasSuffix<T>(result.substr(RV), L"и")) {

        result = result.substr(0, result.length() - 1);
        recalculatePositions();

    }

}


template<typename T>
void Stemmer<T>::thirdStep()
{

    auto suffix = algorithms::findComparableSuffix(result.substr(R2), morph.endings.derivational);
    if(suffix != morph.endings.derivational.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();
        return;

    }

}

template<typename T>
void Stemmer<T>::fourthStep()
{


    auto suffix = algorithms::findComparableSuffix(result.substr(RV), morph.endings.superlative);
    if(suffix != morph.endings.superlative.cend()) {

        result = result.substr(0, result.length() - (*suffix).length());
        recalculatePositions();

    }

    if(algorithms::hasSuffix<T>(result, L"нн")) {

        result = result.substr(0, result.length() - 1);
        recalculatePositions();
        return;

    }

    if(algorithms::hasSuffix<T>(result, L"ь")) {

        result = result.substr(0, result.length() - 1);
        recalculatePositions();
        return;

    }

}

template class Stemmer<wchar_t>;