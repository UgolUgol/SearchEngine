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
	Leaf(size_t hash);
	Iterator next() override;
private:
	size_t offset;
	size_t length;
	Iterator docId;
};

Iterator OperatorAnd::next() {

	return {};
}

Iterator OperatorOr::next() {
	return {};
}

Leaf::Leaf(size_t hash) {

}

Iterator Leaf::next() {
	return {};
}

