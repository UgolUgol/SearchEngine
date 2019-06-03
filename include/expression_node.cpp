#include "expression_node.h"

NotIteratorAdaptor::NotIteratorAdaptor(size_t _length) : length(_length){

	docIds = new size_t[length];
	
	for(size_t idx = 0; idx < length; ++idx) {
		docIds[idx] = idx;
	} 


	currentEntry = SpecialIterator(docIds);

}

NotIteratorAdaptor::~NotIteratorAdaptor() {

	delete docIds;

}

NotIteratorAdaptor& NotIteratorAdaptor::operator=(const boost::optional<DocIdIterator>& iterator) {

	if(iterator) {

		currentEntry = SpecialIterator((*iterator).rawPointer());

	} else {

		currentEntry = boost::none;

	}

	return *this;
}

NotIteratorAdaptor& NotIteratorAdaptor::operator++() {

	if(!currentEntry || **currentEntry == length - 1) {

		currentEntry = boost::none;

	} else {

		++(*currentEntry);
	
	}
	return *this;

}


NotIteratorAdaptor::SpecialIterator& NotIteratorAdaptor::operator*() {

	return *currentEntry;

}

NotIteratorAdaptor::operator boost::optional<DocIdIterator>() {

	if(!currentEntry) {

		return boost::none;
	
	}

	return DocIdIterator((*currentEntry).rawPointer());

}

NotIteratorAdaptor::operator bool() {

	return currentEntry != boost::none;

}

ExpressionNode::ExpressionNode() : currentEntry(boost::none) { }

OperatorAnd::OperatorAnd() : ExpressionNode() { }

OperatorOr::OperatorOr() : ExpressionNode() { }

OperatorNot::OperatorNot() : ExpressionNode(), maxDocId(14923), specialCurrentEntry(maxDocId) { }

OperatorQuote::OperatorQuote(size_t limit, const Index<DefaultIndex>& index) {

	using DocIdType = typename Index<DefaultIndex>::DocId;
	using PositionType = typename Index<DefaultIndex>::Position;

	quoteBlock = std::make_shared<QuoteBlock>(limit);
	docIdBegin = index.coordBegin<DocIdType>();
	positionBegin = index.coordBegin<PositionType>();
	quoteBegin = true;

}

OperatorQuote::OperatorQuote(const OperatorQuote& node) {

	quoteBegin = node.quoteBegin;
	quoteBlock = node.quoteBlock;
	positionBegin = node.positionBegin;
	docIdBegin = node.docIdBegin;

}

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

boost::optional<DocIdIterator> ExpressionNode::current() {

	return currentEntry;

}

void OperatorAnd::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<DocIdIterator> OperatorAnd::next(bool initializate) {

	if(!currentEntry && !initializate) {

		return boost::none;
	
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	while(leftDocId && rightDocId && !algorithms::equal(*leftDocId, *rightDocId)) {
		
		if(algorithms::less(*leftDocId, *rightDocId)) {

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

boost::optional<DocIdIterator> OperatorOr::next(bool initializate) {
	
	if(currentEntry == boost::none && initializate == false) {
		return boost::none;
	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();

	if(leftDocId && rightDocId) {
		
		if(algorithms::less(*leftDocId, *rightDocId)) {

				left->next();
				currentEntry = leftDocId;

		} else if(algorithms::greater(*leftDocId, *rightDocId)) {

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


boost::optional<DocIdIterator> OperatorNot::current() {

	if(!specialCurrentEntry) {

		return boost::none;
	
	}

	return specialCurrentEntry;
}

boost::optional<DocIdIterator> OperatorNot::next(bool initializate) {


	++specialCurrentEntry;

	if(!specialCurrentEntry && !initializate) {

		return boost::none;
		
	}

	if(**specialCurrentEntry < boundaryDocId) {

		return specialCurrentEntry;
		
	}

	do {

		auto docId = left->next();
		if(docId && boundaryDocId != **docId) {

			boundaryDocId = **docId;
			++specialCurrentEntry;

		} else if(!docId){
			
			boundaryDocId = maxDocId;
			++specialCurrentEntry;
			break;
		}

	} while(boundaryDocId <= **specialCurrentEntry);

	
	return specialCurrentEntry;
}


void OperatorQuote::concreteInitializate() {

	currentEntry = next(true);

}

void OperatorQuote::quoteContinue() const {

	quoteBegin = false;

}

void OperatorQuote::resetLowerBound() {

	quoteBlock->lowerBound = 0;

}

boost::optional<DocIdIterator> OperatorQuote::next(bool initializate) {

	if(!currentEntry && !initializate) {
		
		return boost::none;

	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();
	auto previousDifference = quoteBlock->lowerBound;
	auto upperBound = quoteBlock->upperBound;

	while(leftDocId && rightDocId) {

		if(algorithms::equal(*leftDocId, *rightDocId)) {

			auto leftOffset = std::distance(docIdBegin, *leftDocId);
			auto rightOffset = std::distance(docIdBegin, *rightDocId);

			auto leftPosition = positionBegin + leftOffset;
			auto rightPosition = positionBegin + rightOffset;
			auto currentDifference = *leftPosition <= *rightPosition ? 0 : *leftPosition - *rightPosition;
			
			if(currentDifference == 0) {

				leftDocId = left->next();

			} else if(currentDifference <= previousDifference || currentDifference > upperBound) {

				rightDocId = right->next();

			} else if( currentDifference > previousDifference && currentDifference <= upperBound) {

				quoteBlock->lowerBound = currentDifference;
				currentEntry = leftDocId;
				right->next();
				break;

			}

		} else {

			if(algorithms::less(*leftDocId, *rightDocId)) {

				leftDocId = left->next();

			} else {

				rightDocId = right->next();

			}
		}
	}


	if(!leftDocId || ! rightDocId) {

		currentEntry = boost::none;

	}

	if(quoteBegin) {

		resetLowerBound();

	}

	return currentEntry;

}

boost::optional<DocIdIterator> Leaf::next(bool initializate) {
	
	if(position == length) {

		currentEntry = boost::none;
		return currentEntry;

	}

	++position;
	++(*currentEntry);
	return currentEntry;
}
