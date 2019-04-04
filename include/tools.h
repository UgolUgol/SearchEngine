#pragma once

#include <vector>
#include <string>


namespace Tools {

	using Tokens = std::vector<std::wstring>;

	struct ArticleBox {
		std::vector<std::wstring> names;
		std::vector<std::wstring> urls;
		std::vector<Tokens> tokens;
		std::vector<unsigned> size;
	};

	extern std::wstring readData(std::string filename);
	extern void split_regex(Tokens& dest, const wchar_t* src, const wchar_t* breakset, size_t srcSize);
	
}
