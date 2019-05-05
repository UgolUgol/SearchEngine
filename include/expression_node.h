#pragma once
#include <set>
#include <stack>
#include <memory>
#include "traits_engine.h"
#include "index.h"

namespace details {
	enum class OperatorType : size_t;
};

using Iterator = Index<DefaultIndex>::CoordinateBlocksIterator;
using ExpressionPart = std::pair<details::OperatorType, size_t>;

class ExpressionNode {
public:
	void initialize(std::stack<ExpressionPart>& expression);
	virtual Iterator next() = 0;
protected:
	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
};


class OperatorAnd : public ExpressionNode {
public:
	Iterator next() override;
};

class OperatorOr : public ExpressionNode {
public:
	Iterator next() override;
};

class Leaf : public ExpressionNode {
public:
	Iterator next() override;
private:
	size_t offset;
	size_t length;
	Iterator docId;
};

void ExpressionNode::initialize(std::stack<ExpressionPart>& expression) {

	auto operationType = expression.top().first;
	if(operationType == details::OperatorType::_and)  {
		std::cout<<"her"<<std::endl;
	}

}

Iterator OperatorAnd::next() {

	return {};
}


Iterator OperatorOr::next() {
	return {};
}

Iterator Leaf::next() {
	return {};
}

