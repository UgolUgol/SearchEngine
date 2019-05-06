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

ExpressionNode::ExpressionNode() : currentDocId(0) { }

OperatorAnd::OperatorAnd() : ExpressionNode() { }

OperatorOr::OperatorOr() : ExpressionNode() { }

OperatorNot::OperatorNot() : excludedDocId(0), ExpressionNode() { }

Leaf::Leaf(size_t hash, const Index<DefaultIndex>& index) : ExpressionNode() {

	auto hashBlock = algorithms::findInIndex(index.dictionaryBegin(), index.dictionaryEnd(), hash);
	if(hashBlock != index.dictionaryEnd()) {

		offset = index.getOffset(hashBlock);
		length = index.getLength(hashBlock);
		position = 0;
		docId = index.coordBegin() + offset;

	} else {

		offset = length = position = 0;
		docId = boost::none;

	}
}

boost::optional<DocId> ExpressionNode::current() {

	if(currentDocId && *currentDocId == 0) {
		currentDocId = next();
	}

	return currentDocId;
}

boost::optional<DocId> OperatorAnd::next() {

	if(currentDocId == boost::none) {
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
		currentDocId = boost::none;
	} else {
		currentDocId = leftDocId;
	}

	return currentDocId;
}


boost::optional<DocId> OperatorOr::next() {
	
	if(currentDocId == boost::none) {
		return boost::none;
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	if(leftDocId && rightDocId) {
		
		if(*leftDocId < *rightDocId) {

				left->next();
				currentDocId = *leftDocId;

		} else if(*leftDocId > *rightDocId) {

				right->next();
				currentDocId = *rightDocId;

		} else {
			
			left->next();
			right->next();
			currentDocId = *leftDocId;

		}

	} else if(!leftDocId) {
		
		currentDocId = rightDocId;
		right->next();

	} else if(!rightDocId) {
		
		currentDocId = leftDocId;
		left->next();

	}
	return currentDocId;
}

boost::optional<DocId> OperatorNot::next() {

	if(currentDocId == boost::none) {
		return boost::none;
	}

	++(*currentDocId);
	while(currentDocId >= excludedDocId) {

		auto docId = left->next();
		if(docId && excludedDocId != *docId) {

			currentDocId = excludedDocId + 1;
			excludedDocId = *docId;
			
		} else if(!docId) {
			
			currentDocId = boost::none;
			break;

		}
	}

	return currentDocId;
}


boost::optional<DocId> Leaf::current() {

	if(currentDocId && *currentDocId == 0) {
		
		currentDocId = **docId;
		++position;
		++(*docId);

	}

	return currentDocId;
}

boost::optional<DocId> Leaf::next() {
	
	if(position == length) {

		currentDocId = boost::none;
		return currentDocId;

	}

	currentDocId = **docId;
	++position;
	++(*docId);
	return currentDocId;
}

