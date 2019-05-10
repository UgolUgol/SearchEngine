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
	};


	template<> 
	struct TransformatorTraits<wchar_t> {

		static constexpr const wchar_t* space = L" ";
		static constexpr const wchar_t* _and = L"&&";

	};
}


template<typename T> class InputTransformator { };

template<typename T> 
class InputTransformator<std::basic_string<T>> {
public:
	std::basic_string<T> transform(std::basic_string<T>& input);
private:
	std::list<std::basic_string<T>> splitInTokensList(std::basic_string<T>&& input);
	std::basic_string<T> concatenateTokens(std::list<std::basic_string<T>>&& tokens);
};


template<typename T>
std::basic_string<T> InputTransformator<std::basic_string<T>>::transform(std::basic_string<T>& input) {

	return concatenateTokens(splitInTokensList(std::move(input)));

};

template<typename T>
std::list<std::basic_string<T>> InputTransformator<std::basic_string<T>>
::splitInTokensList(std::basic_string<T>&& input) {
	
	std::list<std::basic_string<T>> tokens;
	auto space = details::TransformatorTraits<T>::space;

	boost::algorithm::split(tokens, input, boost::is_any_of(space), boost::token_compress_on);
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

		if(twoOperands || operandAndNegative) {

			token = tokens.insert(std::next(token), _and);

		}
	}

	return boost::algorithm::join(tokens, space);
}