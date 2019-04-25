#pragma once

#include <sstream>
#include <cstring>
namespace Traits {


	template<typename T> 
	struct is_string 
	{
		static constexpr bool value = false;
	};

	template<typename CharT> 
	struct is_string<std::basic_string<CharT>> 
	{
		static constexpr bool value = true;
	};

	template<> 
	struct is_string<char*> 
	{
		static constexpr bool value = true;
	};

	template<>
	struct is_string<const char*> 
	{
		static constexpr bool value = true;
	};


	template<> 
	struct is_string<wchar_t*> 
	{
		static constexpr bool value = true;
	};

	template<>
	struct is_string<const wchar_t*> 
	{
		static constexpr bool value = true;
	};

	template<typename T> inline constexpr bool is_string_v = is_string<T>::value;

}

namespace details 
{

	template<typename T> struct ConstantsTraits { };

	template<> 
	struct ConstantsTraits<char> 
	{

		static const std::map<std::basic_string<char>, size_t> operators;
		static const std::map<std::basic_string<char>, size_t> brackets;
		static constexpr const char delimiter = ' ';

	};
	const std::map<std::basic_string<char>, size_t> ConstantsTraits<char>::operators = 
	{ 
		{"!", 3},
		{"&&", 2},
		{"||", 1}
	};
	const std::map<std::basic_string<char>, size_t> ConstantsTraits<char>::brackets = 
	{ 
		{"(", 0},
		{")", 0}
	};

	template<>
	struct ConstantsTraits<wchar_t> 
	{

		static const std::map<std::basic_string<wchar_t>, size_t> operators;
		static const std::map<std::basic_string<wchar_t>, size_t> brackets;
		static constexpr const wchar_t delimiter = L' ';

	};
	const std::map<std::basic_string<wchar_t>, size_t> ConstantsTraits<wchar_t>::operators = 
	{ 
		{L"!", 3},
		{L"&&", 2},
		{L"||", 1}
	};
	const std::map<std::basic_string<wchar_t>, size_t> ConstantsTraits<wchar_t>::brackets = 
	{ 
		{L"(", 0},
		{L")", 0}
	};
}


namespace functions {


	template<typename T> 
	bool isOperator(const std::basic_string<T>& token) 
	{

		return details::ConstantsTraits<T>::operators.find(token) != details::ConstantsTraits<T>::operators.cend();

	}

	template<typename T>
	bool isOperand(const std::basic_string<T>& token) 
	{

		return !isOperator(token) && 
			   details::ConstantsTraits<T>::brackets.find(token) == details::ConstantsTraits<T>::brackets.cend();

	}

	template<typename T> 
	size_t getPriority(const std::basic_string<T>& token) 
	{ 
		return details::ConstantsTraits<T>::operators.find(token).second;
	}


};

template<typename T> struct ExpressionTraits { };

template<typename T> 
struct ExpressionTraits<std::basic_string<T>> 
{
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};


template<typename T> 
struct ExpressionTraits<const T*> 
{
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};


template<typename T> 
struct ExpressionTraits<T*> {
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};

