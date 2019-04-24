#pragma once

#include <sstream>
namespace Traits {


	template<typename T> 
	struct is_string {
		static constexpr bool value = false;
	};

	template<typename CharT> 
	struct is_string<std::basic_string<CharT>> {
		static constexpr bool value = true;
	};

	template<> 
	struct is_string<char*> {
		static constexpr bool value = true;
	};

	template<>
	struct is_string<const char*> {
		static constexpr bool value = true;
	};

	template<typename T> inline constexpr bool is_string_v = is_string<T>::value;

}



namespace details {

	template<typename T> struct ConstantsTraits { };

	template<> 
	struct ConstantsTraits<char> {

		static constexpr const char* _and = "&&";
		static constexpr const char* _or = "||";
		static constexpr const char* _not = "!";
		static constexpr const char delimiter = ' ';

	};
	template<>
	struct ConstantsTraits<wchar_t> {

		static constexpr const wchar_t* _and = L"&&";
		static constexpr const wchar_t* _or = L"||";
		static constexpr const wchar_t* _not = L"!";
		static constexpr const wchar_t delimiter = L' ';

	};

}

template<typename T> struct ExpressionTraits { };

template<typename T> 
struct ExpressionTraits<std::basic_string<T>> {
	
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;
	using ConstantsTraits = details::ConstantsTraits<T>;

};

template<typename T> 
struct ExpressionTraits<const T*> {
	
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;
	using ConstantsTraits = details::ConstantsTraits<T>;

};


template<typename T> 
struct ExpressionTraits<T*> {
	
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;
	using ConstantsTraits = details::ConstantsTraits<T>;

};

