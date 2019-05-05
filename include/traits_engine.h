#pragma once

#include <sstream>
#include <cstring>
#include <map>
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
	enum class OperatorType : size_t { _not, _and, _or, _leftBracket, _rightBracket, _operand};
	template<typename T> using OperatorsMap = std::map<std::basic_string<T>, std::pair<size_t, OperatorType>>;

	template<typename T>
	struct ConstantsTraits { };
	
	template<> 
	struct ConstantsTraits<char> 
	{

		static const OperatorsMap<char> operators;
		static const OperatorsMap<char> brackets;
		static constexpr const char delimiter = ' ';

	};
	const OperatorsMap<char> ConstantsTraits<char>::operators = 
	{ 
		{"!", {3, OperatorType::_not}},
		{"&&", {2, OperatorType::_and}},
		{"||", {1, OperatorType::_or}}
	};
	const OperatorsMap<char> ConstantsTraits<char>::brackets = 
	{ 
		{"(", {0, OperatorType::_leftBracket}},
		{")", {0, OperatorType::_rightBracket}}
	};


	template<>
	struct ConstantsTraits<wchar_t> 
	{

		static const OperatorsMap<wchar_t> operators;
		static const OperatorsMap<wchar_t> brackets;
		static constexpr const wchar_t delimiter = L' ';

	};
	const OperatorsMap<wchar_t> ConstantsTraits<wchar_t>::operators = 
	{ 
		{L"!", {3, OperatorType::_not}},
		{L"&&", {2, OperatorType::_and}},
		{L"||", {1, OperatorType::_or}}
	};
	const OperatorsMap<wchar_t> ConstantsTraits<wchar_t>::brackets = 
	{ 
		{L"(", {0, OperatorType::_leftBracket}},
		{L")", {0, OperatorType::_rightBracket}}
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
		return details::ConstantsTraits<T>::operators.find(token)->second.first;
	}
	template<typename T>
	details::OperatorType getType(const std::basic_string<T>& token)
	{	
		auto op = details::ConstantsTraits<T>::operators.find(token);
		if(op == details::ConstantsTraits<T>::operators.cend()) {

			op = details::ConstantsTraits<T>::brackets.find(token);

		}
		return op->second.second;
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
struct ExpressionTraits<T*> 
{
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = std::basic_string<T>;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};

