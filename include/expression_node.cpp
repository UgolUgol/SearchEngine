#include "expression_node.h"

void ExpressionNode::initialize(std::stack<ExpressionPart>& expression) {


}

Iterator OperatorAnd::next() {

	return {};
}


Iterator OperatorOr::next() {
	return {};
}


void Leaf::initialize(std::stack<ExpressionPart>& expression) {
	
}

Iterator Leaf::next() {
	return {};
}

