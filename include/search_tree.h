#pragma once
#include <vector>
#include <string>
#include <stack>
#include <map>
#include "traits_engine.h"
#include "expression_node.h"

class SearchTree {
public:
	SearchTree() = default;
	~SearchTree() = default;

	template<typename T> void build(T&& expression);
private:
	std::unique_ptr<ExpressionNode> root;
	template<typename T> auto makeInverseExpression(T&& expression);
	template<typename T> auto convertToInternalView(const std::vector<T>& expression);
};



template<typename T>
void SearchTree::build(T&& expression) {

	auto inverseExpression = makeInverseExpression(std::forward<T>(expression));

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

				while(!operators.empty() &&
				      functions::getPriority(token) <= functions::getPriority(operators.top())) {

					operands.push_back(operators.top());
					operators.pop();

				}

				operators.push(token);

			}
		} else if(functions::isOperand(token)) {

			operands.push_back(std::move(token));

		} else {
			
			if(functions::getType(token) == details::OperatorType::_leftBracket) {

				operators.push(std::move(token));
				
			} else {

				while(!operators.empty() && 
					  functions::getType(operators.top()) != details::OperatorType::_leftBracket) {

					operands.push_back(operators.top());
					operators.pop();

				}
				operators.pop();

			}
		}
	} 

	while(!operators.empty()) {

		operands.push_back(std::move(operators.top()));
		operators.pop();

	}

	return convertToInternalView(operands);
}

template<typename T>
auto SearchTree::convertToInternalView(const std::vector<T>& expression) {

	std::stack<ExpressionPart> resultExpression;
	for(const auto& token : expression) {

		if(functions::isOperand(token)) {

			auto hash = std::hash<T>{}(token);
			resultExpression.push(std::make_pair(details::OperatorType::_operand, hash));
		
		} else {

			resultExpression.push(std::make_pair(functions::getType(token), 0));

		}
	}
	return resultExpression;

}