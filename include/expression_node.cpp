#include "expression_node.h"

ExpressionNode::ExpressionNode() : currentDocId(0) { }

OperatorAnd::OperatorAnd() : ExpressionNode() { }

OperatorOr::OperatorOr() : ExpressionNode() { }

OperatorNot::OperatorNot() : maxDocId(14923), ExpressionNode() { }

Leaf::Leaf(size_t hash, const Index<DefaultIndex>& index) : ExpressionNode() {

	using HashType = typename Index<DefaultIndex>::Hash;
	using DocIdType = typename Index<DefaultIndex>::DocId;

	auto hashBlock = algorithms::findInIndex(index.dictionaryBegin<HashType>(), index.dictionaryEnd<HashType>(), hash);
	if(hashBlock != index.dictionaryEnd<HashType>()) {

		offset = index.getOffset(hashBlock);
		length = index.getLength(hashBlock);
		position = 0;
		docId = index.coordBegin<DocIdType>() + offset;
		currentDocId = **docId;

	} else {

		offset = length = position = 0;
		docId = boost::none;

	}
}

void ExpressionNode::initializate() {

	if(left) {
		left->initializate();
	} 
	if(right) {
		right->initializate();
	}

	concreteInitializate();
}

void ExpressionNode::concreteInitializate() { }

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


void OperatorNot::concreteInitializate() {

	leftExcluded = 0;
	rightExcluded = left->current() ? left->current() : boost::make_optional(maxDocId);
	currentDocId = next();

}


boost::optional<DocId> OperatorNot::current() {

	if(currentDocId <= leftExcluded || currentDocId >= rightExcluded) { 

		currentDocId = next();

	}

	return currentDocId;
}

boost::optional<DocId> OperatorNot::next() {

	if(currentDocId == boost::none) {
		return boost::none;
	}

	++(*currentDocId);
	if(currentDocId > leftExcluded && currentDocId < rightExcluded) {
		return *currentDocId;
	}

	do {

		leftExcluded  = left->current();
		rightExcluded = left->next();

		if(!leftExcluded) {

			leftExcluded = rightExcluded = maxDocId;
			break;

		}

		if(!rightExcluded) {

			rightExcluded = maxDocId;
			break;

		}
	
	} while(*rightExcluded - *leftExcluded < 2);


	if(*rightExcluded - *leftExcluded < 2) {
		
		currentDocId = boost::none;
		return boost::none;

	} else {

		currentDocId = *leftExcluded + 1;
	
	}

	return *currentDocId;
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
