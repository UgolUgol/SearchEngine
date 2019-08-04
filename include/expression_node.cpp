#include "expression_node.h"
#include "varcode.h"


NotIteratorAdaptor::NotIteratorAdaptor(size_t _length) : length(_length){

	docIds = new size_t[length];
	
	for(size_t idx = 0; idx < length; ++idx) {
		docIds[idx] = idx;
	} 

    currentEntry.emplace(SpecialIterator(docIds), 0.0);
}

NotIteratorAdaptor::~NotIteratorAdaptor() {

	delete docIds;

}

NotIteratorAdaptor& NotIteratorAdaptor::operator=(const boost::optional<NodeOutput<DocIdIterator>>& iterator) {

	if(iterator) {

		currentEntry.emplace(SpecialIterator((iterator->docId).rawPointer()), 0.0);

	} else {

		currentEntry = boost::none;

	}

	return *this;
}

NotIteratorAdaptor& NotIteratorAdaptor::operator++() {

	if(!currentEntry || *(currentEntry->docId) == length - 1) {

		currentEntry = boost::none;

	} else {

		++(currentEntry->docId);
	
	}
	return *this;

}


NodeOutput<NotIteratorAdaptor::SpecialIterator>& NotIteratorAdaptor::operator*() {

	return *currentEntry;

}

NotIteratorAdaptor::operator boost::optional<NodeOutput<DocIdIterator>>() {

	if(!currentEntry) {

		return boost::none;
	
	}

	return NodeOutput<DocIdIterator>(DocIdIterator(currentEntry->docId.rawPointer()), 0.0);

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

		currentEntry.emplace(DocIdIterator(reinterpret_cast<void*>(docIds.data())), 0.0);
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

boost::optional<NodeOutput<DocIdIterator>> ExpressionNode::current() {

	return currentEntry;

}

void OperatorAnd::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<NodeOutput<DocIdIterator>> OperatorAnd::next(bool initializate) {

	if(!currentEntry && !initializate) {

		return boost::none;
	
	}

	auto leftEntry = left->current();
	auto rightEntry = right->current();

	while(leftEntry && rightEntry && !algorithms::equal(leftEntry->docId, rightEntry->docId)) {
		
		if(algorithms::less(leftEntry->docId, rightEntry->docId)) {

			leftEntry = left->next();

		} else {

			rightEntry = right->next();

		}
	}
	
	if(!leftEntry || !rightEntry) {

		currentEntry = boost::none;

	} else {

		currentEntry.emplace(leftEntry->docId, leftEntry->rankValue + rightEntry->rankValue);
		left->next();
		right->next();

	}

	return currentEntry;
}

void OperatorOr::concreteInitializate() {

	currentEntry = next(true);

}

boost::optional<NodeOutput<DocIdIterator>> OperatorOr::next(bool initializate) {
	
	if(currentEntry == boost::none && initializate == false) {
		return boost::none;
	}

	auto leftEntry = left->current();
	auto rightEntry = right->current();

	if(leftEntry && rightEntry) {
		
		if(algorithms::less(leftEntry->docId, rightEntry->docId)) {

				left->next();
				currentEntry = leftEntry;

		} else if(algorithms::greater(leftEntry->docId, rightEntry->docId)) {

				right->next();
				currentEntry = rightEntry;

		} else {
			
			left->next();
			right->next();
			currentEntry.emplace(leftEntry->docId, leftEntry->rankValue + rightEntry->rankValue);

		}

	} else if(!leftEntry) {
		
		currentEntry = rightEntry;
		right->next();

	} else if(!rightEntry) {
		
		currentEntry = leftEntry;
		left->next();

	}
	return currentEntry;
}


void OperatorNot::concreteInitializate() {

	if(left->current()) {
		
		boundaryDocId = *(*left->current()).docId;
		
	} else {
		
		boundaryDocId = maxDocId;
	
	}
	specialCurrentEntry = next(true);
}


boost::optional<NodeOutput<DocIdIterator>> OperatorNot::current() {

	if(!specialCurrentEntry) {

		return boost::none;
	
	}

	return specialCurrentEntry;
}

boost::optional<NodeOutput<DocIdIterator>> OperatorNot::next(bool initializate) {


	++specialCurrentEntry;

	if(!specialCurrentEntry && !initializate) {

		return boost::none;
		
	}

	if(*(*specialCurrentEntry).docId < boundaryDocId) {

		return specialCurrentEntry;
		
	}

	do {

		auto leftEntry = left->next();
		if(leftEntry && boundaryDocId != *leftEntry->docId) {

			boundaryDocId = *leftEntry->docId;
			++specialCurrentEntry;

		} else if(!leftEntry){
			
			boundaryDocId = maxDocId;
			++specialCurrentEntry;
			break;
		}

	} while(boundaryDocId <= *(*specialCurrentEntry).docId);

	
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

boost::optional<NodeOutput<DocIdIterator>> OperatorQuote::next(bool initializate) {

	using DocIdType = typename CoordinateIndex<DefaultIndex>::DocId;
	using PositionType = typename CoordinateIndex<DefaultIndex>::Position;

	if(!currentEntry && !initializate) {
		
		return boost::none;

	}

	auto leftEntry = left->current();
	auto rightEntry = right->current();
	auto upperBound = quoteBlock->upperBound();

	bool found = false;
	while(leftEntry && rightEntry && !found) {

		if(algorithms::equal(leftEntry->docId, rightEntry->docId)) {

			auto leftPosition = algorithms::convertIterator<DocIdType, PositionType>(leftEntry->docId);
			auto rightPosition = algorithms::convertIterator<DocIdType, PositionType>(rightEntry->docId);

			auto currentDifference = *leftPosition <= *rightPosition ? 0 : *leftPosition - *rightPosition;
		
			if(currentDifference == 0 || currentDifference <= quoteBlock->lowerBound()) {

				quoteBlock->removeLastDifference();
				leftEntry = left->next();

			} else if(currentDifference > upperBound) {

				rightEntry = right->next();

			} else if( currentDifference > quoteBlock->lowerBound() && currentDifference <= upperBound) {

				quoteBlock->addDifference(currentDifference);
				currentEntry.emplace(leftEntry->docId, leftEntry->rankValue + rightEntry->rankValue);

				right->next();
				found = true;

			}

		} else {

			if(algorithms::less(leftEntry->docId, rightEntry->docId)) {

				quoteBlock->removeLastDifference();
				leftEntry = left->next();

			} else {

				rightEntry = right->next();

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


void Leaf::concreteInitializate()
{

    using DocIdType = typename CoordinateIndex<DefaultIndex>::DocId;
    using TFType = typename CoordinateIndex<DefaultIndex>::TF;
    using DFType = typename CoordinateIndex<DefaultIndex>::DF;

    if(currentEntry) {

        auto tf = algorithms::convertIterator<DocIdType, TFType>(currentEntry->docId);
        auto df = algorithms::convertIterator<DocIdType, DFType>(currentEntry->docId);

        currentEntry->rankValue = StandartRanker::calculate(*tf, *df);

    }

}

boost::optional<NodeOutput<DocIdIterator>> Leaf::next(bool initializate) {


    using DocIdType = typename CoordinateIndex<DefaultIndex>::DocId;
    using TFType = typename CoordinateIndex<DefaultIndex>::TF;
    using DFType = typename CoordinateIndex<DefaultIndex>::DF;

	if(!currentEntry) {

		return boost::none;
	
	}

	++(currentEntry->docId);
	if(std::distance(currentEntry->docId, coordBlockEnd) <= 0) {

		currentEntry = boost::none;

	} else {

	    auto tf = algorithms::convertIterator<DocIdType, TFType>(currentEntry->docId);
	    auto df = algorithms::convertIterator<DocIdType, DFType>(currentEntry->docId);

	    currentEntry->rankValue = StandartRanker::calculate(*tf, *df);
	}

	return currentEntry;
}
