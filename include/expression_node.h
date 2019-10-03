#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include <cmath>
#include "index.h"

using DocIdIterator = CoordinateIndex<DefaultIndex>::DocIdIterator;
using TFIterator = CoordinateIndex<DefaultIndex>::TFIterator;
using DFIterator = CoordinateIndex<DefaultIndex>::DFIterator;
//using PositionIterator = Index<DefaultIndex>::PositionIterator;


template<typename DocIteratorType>
struct NodeOutput {

    DocIteratorType docId;
    double rankValue;

    NodeOutput() = default;
    ~NodeOutput() = default;

    NodeOutput(DocIteratorType docId, double rankValue) : docId(docId), rankValue(rankValue) { }

};


class NotIteratorAdaptor {
public:
	
	using SpecialIterator = IndexIterator<size_t, sizeof(size_t)>;

	NotIteratorAdaptor(size_t);
	~NotIteratorAdaptor();

	NotIteratorAdaptor& operator=(const boost::optional<NodeOutput<DocIdIterator>>& iterator);
	NotIteratorAdaptor& operator++();
	NodeOutput<SpecialIterator>& operator*();

	operator boost::optional<NodeOutput<DocIdIterator>>();
	operator bool();

private:
	boost::optional<NodeOutput<SpecialIterator>> currentEntry;
	size_t* docIds;
	size_t length;
	
};


struct StandartRanker {

    static constexpr std::size_t DocumentsPackSize = 4484323;
    static double calculate(typename TFIterator::value_type tf, typename DFIterator::value_type df)
    {
        return  tf * log10(DocumentsPackSize / static_cast<double>(df + 1));

    }
};

class ExpressionNode {
public:
	ExpressionNode();

	void initializate();
	virtual boost::optional<NodeOutput<DocIdIterator>> current();
	virtual boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) = 0;


	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;

private:
    virtual void concreteInitializate();

protected:
    boost::optional<NodeOutput<DocIdIterator>> currentEntry;
};




class OperatorAnd : public ExpressionNode {
public:
	OperatorAnd();
	boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) override;

private:
	void concreteInitializate() override;
};

class OperatorOr : public ExpressionNode {
public:
	OperatorOr();
	boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) override;

private:
	void concreteInitializate() override;
};

class OperatorNot: public ExpressionNode {
public:
	OperatorNot();
	boost::optional<NodeOutput<DocIdIterator>> current() override;
	boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) override;

private:

	size_t maxDocId;
	NotIteratorAdaptor specialCurrentEntry;
	boost::optional<size_t> boundaryDocId;
	
	void concreteInitializate() override;
};

struct QuoteBlock {
private:
	std::stack<size_t> _lowerBounds;
	size_t _upperBound;

public:	
	QuoteBlock(size_t _upperBound) : _upperBound(_upperBound) { }

	size_t lowerBound() const { return _lowerBounds.empty() ? 0 : _lowerBounds.top(); }
	size_t upperBound() const { return _upperBound; }

	void addDifference(size_t);
	void removeLastDifference(); 
	void reset();
};

class OperatorQuote : public ExpressionNode {
public:

	OperatorQuote(std::size_t limit);
	OperatorQuote(const OperatorQuote& node);

	boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) override;
	void quoteContinue() const;

private:
	mutable bool quoteBegin; 
	std::shared_ptr<QuoteBlock> quoteBlock;

	void resetLowerBound();
	void concreteInitializate() override;
};

class Leaf : public ExpressionNode {
public:
	Leaf(std::size_t hash,
	 	 const DictionaryIndex<DefaultIndex>& dictionary,
	 	 const CoordinateIndex<DefaultIndex>& coordinatFile);
	
	boost::optional<NodeOutput<DocIdIterator>> next(bool initialization = false) override;
private:
	DocIdIterator coordBlockEnd;
	std::vector<size_t> docIds;

    void concreteInitializate() override;
};






