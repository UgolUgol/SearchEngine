#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "traits_engine.h"
#include "index.h"

namespace details {
	enum class OperatorType : size_t;
};

using Iterator = Index<DefaultIndex>::CoordinateBlocksIterator;
using ExpressionPart = std::pair<details::OperatorType, size_t>;

class ExpressionNode {
public:
	virtual boost::optional<Iterator> next() = 0;

	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
};

class OperatorAnd : public ExpressionNode {
public:
	boost::optional<Iterator> next() override;
};

class OperatorOr : public ExpressionNode {
public:
	boost::optional<Iterator> next() override;
};

class Leaf : public ExpressionNode {
public:
	Leaf(size_t hash, const Index<DefaultIndex>& index);
	boost::optional<Iterator> next() override;
private:
	size_t offset;
	size_t length;
	boost::optional<Iterator> docId;
};

boost::optional<Iterator> OperatorAnd::next() {

	return {};
}

boost::optional<Iterator> OperatorOr::next() {
	return {};
}

Leaf::Leaf(size_t hash, const Index<DefaultIndex>& index) {

	auto hashBlock = algorithms::findInIndex(index.dictionaryBegin(), index.dictionaryEnd(), hash);
	if(hashBlock != index.dictionaryEnd()) {

		offset = index.getOffset(hashBlock);
		length = index.getLength(hashBlock);
		docId = index.coordBegin() + offset;

	} else {

		offset = length = 0;
		docId = boost::none;

	}
}

boost::optional<Iterator> Leaf::next() {
	return {};
}

