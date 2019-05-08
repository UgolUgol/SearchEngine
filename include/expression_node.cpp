#include "expression_node.h"

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