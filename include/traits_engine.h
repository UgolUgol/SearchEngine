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


	template<> 
	struct is_string<wchar_t*> {
		static constexpr bool value = true;
	};

	template<>
	struct is_string<const wchar_t*> {
		static constexpr bool value = true;
	};

	template<typename T> inline constexpr bool is_string_v = is_string<T>::value;

}



namespace details {

	template<typename T> struct ConstantsTraits { };

	template<> 
	struct ConstantsTraits<char> {


		using OperatorType = std::pair<const char*, size_t>;

		static constexpr const OperatorType _and = {"&&", 2};
		static constexpr const OperatorType _or = {"||", 1};
		static constexpr const OperatorType _not = {"!", 3};
		static constexpr const OperatorType _leftBracket = {"(", 0};
		static constexpr const OperatorType _rightBracket = {")", 0};
		static constexpr const char delimiter = ' ';

	};

	template<>
	struct ConstantsTraits<wchar_t> {

		using OperatorType = std::pair<const wchar_t*, size_t>;

		static constexpr const OperatorType _and = {L"&&", 2};
		static constexpr const OperatorType _or = {L"||", 1};
		static constexpr const OperatorType _not = {L"!", 3};
		static constexpr const OperatorType _leftBracket = {L"(", 0};
		static constexpr const OperatorType _rightBracket = {L")", 0};
		static constexpr const wchar_t delimiter = L' ';

	};

	template<typename T> 
	bool isOperator(const std::basic_string<T>& token) {

		return token == ConstantsTraits<T>::_and ||
		 	   token == ConstantsTraits<T>::_or ||
		 	   token == ConstantsTraits<T>::_not;

	}

	template<typename T>
	bool isOperand(const std::basic_string<T>& token) {

		return !isOperator(token) &&
		 		token != ConstantsTraits<T>::_leftBracket &&
		 		token != ConstantsTraits<T>::_rightBracket;

	}


}

template<typename T> struct ExpressionTraits { };

template<typename T> 
struct ExpressionTraits<std::basic_string<T>> {
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = typename ConstantsTraits::OperatorType;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};

template<typename T> 
struct ExpressionTraits<const T*> {
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = typename ConstantsTraits::OperatorType;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};


template<typename T> 
struct ExpressionTraits<T*> {
	
	using ConstantsTraits = details::ConstantsTraits<T>;
	using OperatorType = typename ConstantsTraits::OperatorType;
	using OperandType = std::basic_string<T>;
	using UnderlyingType = T;
	using HandlerType = std::basic_stringstream<T>;

};

