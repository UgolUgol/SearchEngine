#pragma once

#include <string>
#include <vector>
#include <regex>
#include <list>
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
		static constexpr const char* binaryOperatorsRegex = "(&&|\\|\\||\\(\\))";
		static constexpr const char* binaryOperatorsRegexReplace = " $1 ";

	};


	template<> 
	struct TransformatorTraits<wchar_t> {

		static constexpr const wchar_t* space = L" ";
		static constexpr const wchar_t* _and = L"&&";
		static constexpr const wchar_t* binaryOperatorsRegex = L"(&&|\\|\\||\\!|\\(|\\))";
		static constexpr const wchar_t* binaryOperatorsRegexReplace = L" $1 ";

	};
}


template<typename T> class InputTransformator { };

template<typename T> 
class InputTransformator<std::basic_string<T>> {
public:
	std::basic_string<T> transform(std::basic_string<T>& input);
private:
	std::basic_string<T>&& correctUnspacedParts(std::basic_string<T>&& input);
	std::list<std::basic_string<T>> splitInTokensList(std::basic_string<T>&& input);
	std::basic_string<T> concatenateTokens(std::list<std::basic_string<T>>&& tokens);
};


template<typename T>
std::basic_string<T> InputTransformator<std::basic_string<T>>::transform(std::basic_string<T>& input) {

	return concatenateTokens(splitInTokensList(correctUnspacedParts(std::move(input))));

};

template<typename T>
std::basic_string<T>&& InputTransformator<std::basic_string<T>>
::correctUnspacedParts(std::basic_string<T>&& input) {

	std::basic_regex<T> binaryReg(details::TransformatorTraits<T>::binaryOperatorsRegex);	
	input = std::regex_replace(input, binaryReg, details::TransformatorTraits<T>::binaryOperatorsRegexReplace);
	
	return std::move(input);
}


template<typename T>
std::list<std::basic_string<T>> InputTransformator<std::basic_string<T>>
::splitInTokensList(std::basic_string<T>&& input) {
	
	std::list<std::basic_string<T>> tokens;
	auto space = details::TransformatorTraits<T>::space;
	auto isempty = [](const auto& token) { return token.empty(); };

	boost::algorithm::split(tokens, input, boost::is_any_of(space), boost::token_compress_on);
	
	auto it = std::remove_if(tokens.begin(), tokens.end(), isempty);
	tokens.erase(it, tokens.end());

	return tokens;

}

template<typename T>
std::basic_string<T> InputTransformator<std::basic_string<T>>
::concatenateTokens(std::list<std::basic_string<T>>&& tokens) {

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

		if(twoOperands || operandAndNegative || operandAndBracket || bracketAndOperand || bracketAndNegative || twoBrackets) {

			token = tokens.insert(std::next(token), _and);

		}
	}

	return boost::algorithm::join(tokens, space);
}
