#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "index.h"

using Iterator = Index<DefaultIndex>::DocIdIterator;
using DocId = Iterator::value_type;


class ExpressionNode {
public:
	ExpressionNode();

	virtual boost::optional<DocId> current();
	virtual boost::optional<DocId> next() = 0;

	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
protected:
	boost::optional<DocId> currentDocId;
};

class OperatorAnd : public ExpressionNode {
public:
	OperatorAnd();
	boost::optional<DocId> next() override;
};

class OperatorOr : public ExpressionNode {
public:
	OperatorOr();
	boost::optional<DocId> next() override;
};

class OperatorNot: public ExpressionNode {
public:
	OperatorNot();
	boost::optional<DocId> next() override;

private:
	size_t excludedDocId;
};

class Leaf : public ExpressionNode {
public:
	Leaf(size_t hash, const Index<DefaultIndex>& index);

	boost::optional<DocId> current() override;
	boost::optional<DocId> next() override;

private:
	size_t offset;
	size_t length;
	size_t position;
	boost::optional<Iterator> docId;
};



