#include "expression_node.h"

NotIteratorAdaptor::NotIteratorAdaptor() : docId(0) {

	currentEntry = Iterator(&docId);

}

NotIteratorAdaptor& NotIteratorAdaptor::operator=(const boost::optional<Iterator>& iterator) {

	if(iterator) {

		docId = **iterator;
		currentEntry = Iterator(&docId);

	} else {

		currentEntry = boost::none;

	}

	return *this;
}

NotIteratorAdaptor& NotIteratorAdaptor::operator++() {

	++docId;
	return *this;

}


Iterator& NotIteratorAdaptor::operator*() {

	return *currentEntry;

}

NotIteratorAdaptor::operator boost::optional<Iterator>() {

	return currentEntry;

}

NotIteratorAdaptor::operator bool() {

	return currentEntry != boost::none;

}

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
		position = 1;
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

	if(!currentEntry && !initializate) {

		return boost::none;
	
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	while(leftDocId && rightDocId && **leftDocId != **rightDocId) {
	
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
		left->next();
		right->next();

	}

	return currentEntry;
}

void OperatorOr::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<Iterator> OperatorOr::next(bool initializate) {
	
	if(currentEntry == boost::none && initializate == false) {
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

	if(left->current()) {
		
		boundaryDocId = **left->current();
		
	} else {
		
		boundaryDocId = maxDocId;
	
	}
	specialCurrentEntry = next(true);
}


boost::optional<Iterator> OperatorNot::current() {

	if(!specialCurrentEntry) {

		return boost::none;
	
	}

	return specialCurrentEntry;
}

boost::optional<Iterator> OperatorNot::next(bool initializate) {


	if(!specialCurrentEntry && !initializate) {

		return boost::none;
		
	}


	++specialCurrentEntry;
	if(**specialCurrentEntry < boundaryDocId) {

		return specialCurrentEntry;
		
	}
	
	do {

		if(left->next() && boundaryDocId != **left->current()) {

			boundaryDocId = **left->current();
			++specialCurrentEntry;

		} else if(!left->next()){

			boundaryDocId = maxDocId;
			++specialCurrentEntry;
			break;
		}

	} while(boundaryDocId <= **specialCurrentEntry);

	if(boundaryDocId <= **specialCurrentEntry) {

		specialCurrentEntry = boost::none;

	}

	return specialCurrentEntry;
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
