#pragma once
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <traits_engine.h>

class SearchTree {
public:
	SearchTree() = default;
	~SearchTree() = default;

	template<typename T> void build(T&& expression);
private:
	template<typename T> T makeInverseExpression(T&& expression);
};



template<typename T>
void SearchTree::build(T&& expression) {

	decltype(auto) inverseExpression = makeInverseExpression(std::forward<T>(expression));

}

template<typename T>
T SearchTree::makeInverseExpression(T&& expression) {

	using ClearType = std::decay_t<T>;

	std::stack<typename ExpressionTraits<ClearType>::OperatorType> operators;
	std::stack<typename ExpressionTraits<ClearType>::OperandType> operands;

	typename ExpressionTraits<ClearType>::HandlerType handler(expression);
	typename ExpressionTraits<ClearType>::OperandType token;

	while(std::getline(handler, token, ExpressionTraits<ClearType>::ConstantsTraits::delimiter)) {
		std::cout<<token<<std::endl;
	} 


	return std::forward<T>(expression);
}

