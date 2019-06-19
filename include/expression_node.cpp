#include "expression_node.h"
#include "varcode.h"

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

OperatorQuote::OperatorQuote(size_t limit) {

	quoteBlock = std::make_shared<QuoteBlock>(limit);
	quoteBegin = true;

}

OperatorQuote::OperatorQuote(const OperatorQuote& node) {

	quoteBegin = node.quoteBegin;
	quoteBlock = node.quoteBlock;

}

Leaf::Leaf(std::size_t hash,
	 	   const DictionaryIndex<DefaultIndex>& dictionary,
	 	   const CoordinateIndex<DefaultIndex>& coordinatFile) : ExpressionNode() {

	using HashType = typename DictionaryIndex<DefaultIndex>::Hash;
	using OffsetType = typename DictionaryIndex<DefaultIndex>::OffsetInfo;
	using LengthType = typename DictionaryIndex<DefaultIndex>::LengthInfo;
	using CompressedCoordBlock = typename CoordinateIndex<DefaultIndex>::CompressedCoordinateFile;

	auto hashBlock = algorithms::findInIndex(dictionary.begin<HashType>(), dictionary.end<HashType>(), hash);

	if(hashBlock != dictionary.end<HashType>()) {

		auto offset = algorithms::convertIterator<HashType, OffsetType>(hashBlock);
		auto length = algorithms::convertIterator<HashType, LengthType>(hashBlock);

		auto coordBlock = coordinatFile.begin<CompressedCoordBlock>() + *offset;
		docIds = Varcode::decompress<std::size_t>(coordBlock.getUnderlyingPointer(), *length);

		currentEntry = DocIdIterator(reinterpret_cast<void*>(docIds.data()));
		coordBlockEnd = DocIdIterator(reinterpret_cast<void*>(docIds.data() + docIds.size()));


	} else {

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


void QuoteBlock::addDifference(size_t difference)
{
	_lowerBounds.push(difference);
}
void QuoteBlock::removeLastDifference()
{
	if(!_lowerBounds.empty()) {

		_lowerBounds.pop();

	}

}
void QuoteBlock::reset() {

	while(!_lowerBounds.empty()) {

		_lowerBounds.pop();

	}

}

boost::optional<DocIdIterator> OperatorQuote::next(bool initializate) {

	using DocIdType = typename CoordinateIndex<DefaultIndex>::DocId;
	using PositionType = typename CoordinateIndex<DefaultIndex>::Position;

	if(!currentEntry && !initializate) {
		
		return boost::none;

	}

	auto leftDocId = left->current();
	auto rightDocId = right->current();
	auto upperBound = quoteBlock->upperBound();

	bool found = false;
	while(leftDocId && rightDocId && !found) {

		if(algorithms::equal(*leftDocId, *rightDocId)) {

			auto leftPosition = algorithms::convertIterator<DocIdType, PositionType>(*leftDocId);
			auto rightPosition = algorithms::convertIterator<DocIdType, PositionType>(*rightDocId);

			auto currentDifference = *leftPosition <= *rightPosition ? 0 : *leftPosition - *rightPosition;
		
			if(currentDifference == 0 || currentDifference <= quoteBlock->lowerBound()) {

				quoteBlock->removeLastDifference();
				leftDocId = left->next();

			} else if(currentDifference > upperBound) {

				rightDocId = right->next();

			} else if( currentDifference > quoteBlock->lowerBound() && currentDifference <= upperBound) {

				quoteBlock->addDifference(currentDifference);
				currentEntry = leftDocId;
				right->next();
				found = true;

			}

		} else {

			if(algorithms::less(*leftDocId, *rightDocId)) {

				quoteBlock->removeLastDifference();
				leftDocId = left->next();

			} else {

				rightDocId = right->next();

			}
		}
	}

	if(found && quoteBegin) {

		quoteBlock->reset();

	} else if(!found) {

		currentEntry = boost::none;

	}

	return currentEntry;
}

boost::optional<DocIdIterator> Leaf::next(bool initializate) {

	if(!currentEntry) {

		return boost::none;
	
	}

	++(*currentEntry);
	if(std::distance(*currentEntry, coordBlockEnd) <= 0) {

		currentEntry = boost::none;

	}

	return currentEntry;
}
