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
	template<typename T> auto makeInverseExpression(T&& expression);
};



template<typename T>
void SearchTree::build(T&& expression) {

	decltype(auto) inverseExpression = makeInverseExpression(std::forward<T>(expression));

}

template<typename T>
auto SearchTree::makeInverseExpression(T&& expression) {

	using ClearType = std::decay_t<T>;

	std::stack<typename ExpressionTraits<ClearType>::OperatorType> operators;
	std::vector<typename ExpressionTraits<ClearType>::OperandType> operands;

	typename ExpressionTraits<ClearType>::HandlerType handler(expression);
	typename ExpressionTraits<ClearType>::OperandType token;

	while(std::getline(handler, token, ExpressionTraits<ClearType>::ConstantsTraits::delimiter)) {
		
		if(functions::isOperator(token)) {

			if(operators.empty()) {

				operators.push(std::move(token));

			} else {

				while(!operators.empty() && functions::getPriority(token) <= functions::getPriority(operators.top())) {

					operands.push_back(operators.top());
					operators.pop();

				}

				operators.push(token);

			}
		} else if(functions::isOperand(token)) {

			operands.push_back(std::move(token));

		} else {
			
		}
	} 

	while(!operators.empty()) {

		operands.push_back(std::move(operators.top()));
		operators.pop();

	}


	return operands;
}

