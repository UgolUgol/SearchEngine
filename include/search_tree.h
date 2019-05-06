#pragma once
#include <vector>
#include <string>
#include <stack>
#include "expression_node.h"
#include "traits_engine.h"

using ExpressionPart = std::pair<details::OperatorType, size_t>;

class SearchTree {
public:
	SearchTree();
	~SearchTree() = default;

	template<typename T> void build(T&& expression);
	std::set<size_t> extractResults();

private:
	Index<DefaultIndex> index;
	std::unique_ptr<ExpressionNode> root;

	template<typename T> auto makeInverseExpression(T&& expression);
	template<typename T> auto convertToInternalView(const std::vector<T>& expression);
	std::unique_ptr<ExpressionNode> makeTreeFromExpression(std::stack<ExpressionPart>& expression);
};

SearchTree::SearchTree() : index("../index/files/dict.bin", "../index/files/coord.bin") {

}

template<typename T>
void SearchTree::build(T&& expression) {

	auto inverseExpression = makeInverseExpression(std::forward<T>(expression));
	root = makeTreeFromExpression(inverseExpression);

}

std::set<size_t> SearchTree::extractResults() {
	
	auto currentDocId = root->next();
	std::set<size_t> docIds;
	
	while(currentDocId != boost::none) {

		docIds.insert(*currentDocId);
		currentDocId = root->next();

	}
	return docIds;
}

std::unique_ptr<ExpressionNode> SearchTree::makeTreeFromExpression(std::stack<ExpressionPart>& expression) {

	std::unique_ptr<ExpressionNode> node;
	auto nodeType = expression.top().first;
	auto hash = expression.top().second;
	expression.pop();

	if(nodeType == details::OperatorType::_and) {

		node = std::make_unique<OperatorAnd>();
		node->left = makeTreeFromExpression(expression);
		node->right = makeTreeFromExpression(expression);

	} else if(nodeType == details::OperatorType::_or) {

		node = std::make_unique<OperatorOr>();
		node->left = makeTreeFromExpression(expression);
		node->right = makeTreeFromExpression(expression);

	} else if(nodeType == details::OperatorType::_not) {

		node = std::make_unique<OperatorNot>();
		node->left = makeTreeFromExpression(expression);

	} else if(nodeType == details::OperatorType::_operand) {

		node = std::make_unique<Leaf>(hash, index);
		node->left = nullptr;
		node->right = nullptr;

	}

	return node;
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