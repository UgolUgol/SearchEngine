#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "index.h"

namespace details {
	enum class OperatorType : size_t;
};

using Iterator = Index<DefaultIndex>::CoordinateBlocksIterator;
using DocId = Iterator::value_type;


class ExpressionNode {
public:
	ExpressionNode();
	virtual boost::optional<DocId> next() = 0;

	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;
protected:
	bool isOver;
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
	size_t currentDocId;
	size_t excludedDocId;
};

class Leaf : public ExpressionNode {
public:
	Leaf(size_t hash, const Index<DefaultIndex>& index);
	boost::optional<DocId> next() override;

private:
	size_t offset;
	size_t length;
	size_t current;
	boost::optional<Iterator> docId;
};

ExpressionNode::ExpressionNode() : isOver(false) { }
OperatorAnd::OperatorAnd() : ExpressionNode() { }
OperatorOr::OperatorOr() : ExpressionNode() { }
OperatorNot::OperatorNot() : currentDocId(0), excludedDocId(0), ExpressionNode() { }

Leaf::Leaf(size_t hash, const Index<DefaultIndex>& index) : ExpressionNode() {

	auto hashBlock = algorithms::findInIndex(index.dictionaryBegin(), index.dictionaryEnd(), hash);
	if(hashBlock != index.dictionaryEnd()) {

		offset = index.getOffset(hashBlock);
		length = index.getLength(hashBlock);
		current = 0;
		docId = index.coordBegin() + offset;

	} else {

		offset = length = current = 0;
		docId = boost::none;

	}
}

boost::optional<DocId> OperatorAnd::next() {

	if(isOver) {
		return boost::none;
	}

	auto leftDocId = left->next();
	auto rightDocId = right->next();

	while(leftDocId && rightDocId && (*leftDocId) != (*rightDocId)) {
		
		if(*leftDocId < *rightDocId) {

			leftDocId = left->next();

		} else {

			rightDocId = right->next();

		}
	}

	if(!leftDocId || !rightDocId) {

		isOver = true;
		return boost::none;
	
	}

	return leftDocId;
}

boost::optional<DocId> OperatorOr::next() {
	
	if(isOver) {
		return boost::none;
	}

	auto leftDocId = left->next();
	if(leftDocId != boost::none) {

		return leftDocId;

	}

	auto rightDocId = right->next();
	if(rightDocId != boost::none) {

		return rightDocId;

	}

	isOver = true;
	return boost::none;
}

boost::optional<DocId> OperatorNot::next() {

	if(isOver) {
		return boost::none;
	}

	while(currentDocId == excludedDocId) {

		auto docId = left->next();
		
		if(docId && excludedDocId != *docId) {

			currentDocId = excludedDocId + 1;
			excludedDocId = *docId;
			

		} else if(!docId) {
			
			isOver = true;
			return boost::none;

		}

	}
	return currentDocId++;

}

boost::optional<DocId> Leaf::next() {
	
	if(current == length) {

		return boost::none;

	}

	++current;
	return *((*docId)++);
}

