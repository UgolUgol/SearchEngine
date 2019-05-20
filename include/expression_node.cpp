#include "expression_node.h"

ExpressionNode::ExpressionNode() : currentEntry(boost::none) { }

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
		currentEntry = index.coordBegin<DocIdType>() + offset;

	} else {

		offset = length = position = 0;
		currentEntry = boost::none;

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

boost::optional<Iterator> ExpressionNode::current() {

	return currentEntry;

}

void OperatorAnd::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<Iterator> OperatorAnd::next(bool initializate) {

	if(currentEntry == boost::none && initializate == false) {

		return boost::none;
	
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	while(leftDocId && rightDocId && std::equal(*leftDocId, *leftDocId + 1, *rightDocId)) {
		
		if(**leftDocId < **rightDocId) {

			leftDocId = left->next();

		} else {

			rightDocId = right->next();

		}
	}

	if(!leftDocId || !rightDocId) {

		currentEntry = boost::none;

	} else {

		currentEntry = leftDocId;

	}

	return currentEntry;
}

void OperatorOr::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<Iterator> OperatorOr::next(bool initializate) {
	
	if(currentDocId == boost::none && initializate == false) {
		return boost::none;
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	if(leftDocId && rightDocId) {
		
		if(**leftDocId < **rightDocId) {

				left->next();
				currentEntry = leftDocId;

		} else if(**leftDocId > **rightDocId) {

				right->next();
				currentEntry = rightDocId;

		} else {
			
			left->next();
			right->next();
			currentEntry = leftDocId;

		}

	} else if(!leftDocId) {
		
		currentEntry = rightDocId;
		right->next();

	} else if(!rightDocId) {
		
		currentEntry = leftDocId;
		left->next();

	}
	return currentEntry;
}


void OperatorNot::concreteInitializate() {

	leftExcluded = 0;
	rightExcluded = left->current() ? left->current() : boost::make_optional(maxDocId);
	currentEntry = next(true);

}


boost::optional<DocId> OperatorNot::current() {

	if(!currentEntry) {

		return boost::none;
	
	}

	auto docId = **currentEntry;
	if(docId <= leftExcluded || docId >= rightExcluded) { 

		currentEntry = next();

	}

	return currentEntry;
}

boost::optional<DocId> OperatorNot::next(bool initializate) {

	if(currentDocId == boost::none && initializate == false) {

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

boost::optional<Iterator> Leaf::next(bool initializate) {
	
	if(position == length) {

		currentEntry = boost::none;
		return currentEntry;

	}

	++position;
	++(*currentEntry);
	return currentEntry;
}
