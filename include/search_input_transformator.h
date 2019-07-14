#pragma once

#include <string>
#include <vector>
#include <regex>
#include <list>
#include <stemmer.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "traits_engine.h"

namespace details {
	template<typename T> struct TransformatorTraits { };

	template<> 
	struct TransformatorTraits<char> {

		static constexpr const char* space = " ";
		static constexpr const char* _and = "&&";
		static constexpr const char* _quoteUnion = "+";
		static constexpr const char* _quote = "'";
		static constexpr const char* _backslash = "\\";
		static constexpr const char* binaryOperatorsRegex = "(\\\\[0-9]*|'|&&|\\|\\||\\(\\))";
		static constexpr const char* binaryOperatorsRegexReplace = " $1 ";

	};

	template<> 
	struct TransformatorTraits<wchar_t> {

		static constexpr const wchar_t* space = L" ";
		static constexpr const wchar_t* _and = L"&&";
		static constexpr const wchar_t* _quoteUnion = L"+";
		static constexpr const wchar_t* _quote = L"'";
		static constexpr const wchar_t* _backslash = L"\\";
		static constexpr const wchar_t* binaryOperatorsRegex = L"(\\\\[0-9]*|'|&&|\\|\\||\\!|\\(|\\))";
		static constexpr const wchar_t* binaryOperatorsRegexReplace = L" $1 ";

	};

}

namespace functions {

	template<typename T> std::basic_string<T> numericToString(std::size_t);
	
	template<> std::wstring numericToString<wchar_t>(std::size_t number) 
	{ 
		return std::to_wstring(number); 
	}

	template<> std::string numericToString<char>(std::size_t number) 
	{ 
		return std::to_string(number); 
	}

	template<typename T> bool isQuote(const std::basic_string<T>& token)
	{	
		return token == details::TransformatorTraits<T>::_quote;
	}
}


template<typename T> using StringType = std::basic_string<T>;
template<typename T> using ListType = std::list<StringType<T>>;


template<typename T> class InputTransformator { };

template<typename T> 
class InputTransformator<StringType<T>> {

public:
	StringType<T> transform(StringType<T>& input);

private:
	StringType<T> correctUnspacedParts(StringType<T>&& input);
	ListType<T> splitInTokensList(StringType<T>&& input);
	ListType<T> morphologyConversion(ListType<T>&& tokens);
	ListType<T> quotesConversion(ListType<T>&& tokens);
	StringType<T> concatenateTokens(ListType<T>&& tokens);
};


template<typename T>
StringType<T> InputTransformator<StringType<T>>::transform(StringType<T>& input) {

	return concatenateTokens(quotesConversion(morphologyConversion(splitInTokensList(correctUnspacedParts(std::move(input))))));

};


template<typename T>
StringType<T> InputTransformator<StringType<T>>
::correctUnspacedParts(StringType<T>&& input) {

	std::basic_regex<T> binaryReg(details::TransformatorTraits<T>::binaryOperatorsRegex);	
	input = std::regex_replace(input, binaryReg, details::TransformatorTraits<T>::binaryOperatorsRegexReplace);

	return std::move(input);
}


template<typename T>
ListType<T> InputTransformator<StringType<T>>
::splitInTokensList(StringType<T>&& input) {

	ListType<T> tokens;
	auto space = details::TransformatorTraits<T>::space;
	auto isempty = [](const auto& token) { return token.empty(); };

	boost::algorithm::split(tokens, input, boost::is_any_of(space), boost::token_compress_on);
	
	auto it = std::remove_if(tokens.begin(), tokens.end(), isempty);
	tokens.erase(it, tokens.end());

	return tokens;

}

template<typename T>
ListType<T> InputTransformator<StringType<T>>::morphologyConversion(ListType<T> &&tokens)
{

    for(auto& token : tokens) {

        Stemmer<T> st(token);
        token = st.normalize();

    }

    return tokens;
}

template<typename T>
ListType<T> InputTransformator<StringType<T>>
::quotesConversion(ListType<T>&& tokens)
{
	auto quoteUnion = details::TransformatorTraits<T>::_quoteUnion;
	auto backslash = details::TransformatorTraits<T>::_backslash;
	auto quote = details::TransformatorTraits<T>::_quote;

	for(auto token = tokens.begin(); token != tokens.end(); ++token) {

		if(*token == quote) {
			
			size_t distance = 0;
			token = tokens.erase(token);

			while(std::distance(token, tokens.end()) > 1 && 
				  !functions::isOperator(*std::next(token)) && !functions::isQuote(*std::next(token))) {

				token = tokens.insert(std::next(token), quoteUnion);
				++token;
				++distance;

			}
			
			++token;
			if(token != tokens.end() && functions::isQuote(*token)) {

				token = tokens.erase(token);

			}

		 	if(token == tokens.end() || (*token).substr(0, 1) != backslash) {
				token = tokens.insert(token, backslash + functions::numericToString<T>(distance));
				++token;
			}
		}
	}

	return std::move(tokens);
}


template<typename T>
StringType<T> InputTransformator<StringType<T>>
::concatenateTokens(ListType<T>&& tokens) {

	auto space = details::TransformatorTraits<T>::space;
	auto _and = details::TransformatorTraits<T>::_and;

	for(auto token = tokens.begin(); token != std::prev(tokens.end()); ++token) {

		auto nextToken = std::next(token);
		bool twoOperands = functions::isOperand(*token) && functions::isOperand(*nextToken);

		bool operandAndNegative = functions::isOperand(*token) && functions::isOperator(*nextToken) &&
								  (functions::getType(*nextToken) == details::OperatorType::_not);

		bool operandAndBracket = functions::isOperand(*token) &&  functions::isBracket(*nextToken) &&
								 (functions::getType(*nextToken) == details::OperatorType::_leftBracket);

		bool bracketAndOperand = functions::isBracket(*token) && functions::isOperand(*nextToken) &&
								 (functions::getType(*token) == details::OperatorType::_rightBracket);
		
		bool bracketAndNegative = functions::isBracket(*token) && functions::isOperator(*nextToken) &&
								  (functions::getType(*token) == details::OperatorType::_rightBracket) && 
								  (functions::getType(*nextToken) == details::OperatorType::_not);

		bool twoBrackets = functions::isBracket(*token) && functions::isBracket(*nextToken) && 
						   (functions::getType(*token) == details::OperatorType::_rightBracket)	&&
						   (functions::getType(*nextToken) == details::OperatorType::_leftBracket);

		bool quoteLimitAndOperand = functions::isQuoteLimit(*token) && functions::isOperand(*nextToken);
		
		if(twoOperands || operandAndNegative || operandAndBracket ||
		   bracketAndOperand || bracketAndNegative || twoBrackets || quoteLimitAndOperand) {

			token = tokens.insert(std::next(token), _and);

		}
	}

	return boost::algorithm::join(tokens, space);
}
