#include "stemmer.h"
#include <algorithm>

const std::vector<wchar_t>
Morphology::vowels { L'а', L'е', L'и', L'о', L'у', L'ы', L'э', L'ю', L'я'};

const std::vector<std::wstring_view>
Morphology::Endings::perfectiveGerund1 ({L"в", L"вши", L"вшись"});

const std::vector<std::wstring_view>
Morphology::Endings::perfectiveGerund2 ({L"ив", L"ивши", L"ившись", L"ыв", L"ывши", L"ывшись"});

const std::vector<std::wstring_view>
Morphology::Endings::adjective ({
        L"ее", L"ие", L"ые", L"ое", L"ими", L"ыми", L"ей", L"ий", L"ый",
        L"ой", L"ем", L"им", L"ым", L"ом", L"его", L"ого", L"ему", L"ому",
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
        L"ла", L"на", L"ете", L"йте", L"ли", L"й", L"л", L"ем", L"н", L"ло", L"но", L"ет", L"ют", L"ны", L"ть", L"ешь", L"нно"
});

const std::vector<std::wstring_view>
Morphology::Endings::verb2 ({
        L"ила", L"ыла", L"ена", L"ейте", L"уйте", L"ите", L"или", L"ыли", L"ей", L"уй", L"ил", L"ыл",
        L"им", L"ым", L"ен", L"ило", L"ыло", L"ено", L"ят", L"ует", L"уют", L"ит", L"ыт", L"ены", L"ить",
        L"ыть", L"ишь", L"ую", L"ю"
});

const std::vector<std::wstring_view>
Morphology::Endings::noun ({
        L"а", L"ев", L"ов", L"ие", L"ье", L"е", L"иями", L"ями", L"ами", L"еи", L"ии", L"и",
        L"ией", L"ей", L"ой", L"ий", L"й", L"иям", L"ям", L"ием", L"ем", L"ам", L"ом", L"о", L"у", L"ах",
        L"иях", L"ях", L"ы", L"ь", L"ию", L"ью", L"ю", L"ия", L"ья", L"я"
});

const std::vector<std::wstring_view>
Morphology::Endings::superlative ({ L"ейш", L"ейше" });

const std::vector<std::wstring_view>
Morphology::Endings::derivational ({ L"ост", L"ость" });


template<typename T>
Stemmer<T>::Stemmer(const std::basic_string_view<T> word) : word(word)
{

    auto firstVowel = std::find_first_of(word.cbegin(), word.cend(), morph.vowels.cbegin(), morph.vowels.cend());
    auto position = std::distance(word.cbegin(), firstVowel) + 1;

    RV = word.substr(position, std::string::npos);
}

template class Stemmer<wchar_t>;