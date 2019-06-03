#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "index.h"

using DocIdIterator = Index<DefaultIndex>::DocIdIterator;
using PositionIterator = Index<DefaultIndex>::PositionIterator;


class NotIteratorAdaptor {
public:
	
	using SpecialIterator = IndexIterator<size_t, sizeof(size_t)>;

	NotIteratorAdaptor(size_t);
	~NotIteratorAdaptor();

	NotIteratorAdaptor& operator=(const boost::optional<DocIdIterator>& iterator);
	NotIteratorAdaptor& operator++();
	SpecialIterator& operator*();

	operator boost::optional<DocIdIterator>();
	operator bool();

private:
	boost::optional<SpecialIterator> currentEntry;
	size_t* docIds;
	size_t length;
	
};



class ExpressionNode {
public:
	ExpressionNode();

	void initializate();
	virtual boost::optional<DocIdIterator> current();
	virtual boost::optional<DocIdIterator> next(bool initialization=false) = 0;


	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;

protected:
	boost::optional<DocIdIterator> currentEntry;
private:
	virtual void concreteInitializate();
};

class OperatorAnd : public ExpressionNode {
public:
	OperatorAnd();
	boost::optional<DocIdIterator> next(bool initialization=false) override;

private:
	void concreteInitializate() override;
};

class OperatorOr : public ExpressionNode {
public:
	OperatorOr();
	boost::optional<DocIdIterator> next(bool initialization=false) override;

private:
	void concreteInitializate() override;
};

class OperatorNot: public ExpressionNode {
public:
	OperatorNot();
	boost::optional<DocIdIterator> current() override;
	boost::optional<DocIdIterator> next(bool initialization=false) override;

private:

	size_t maxDocId;
	NotIteratorAdaptor specialCurrentEntry;
	boost::optional<size_t> boundaryDocId;
	
	void concreteInitializate() override;
};

struct QuoteBlock {
	size_t lowerBound;
	size_t upperBound;
	QuoteBlock(size_t _upperBound) : lowerBound(0), upperBound(_upperBound) { }
};

class OperatorQuote : public ExpressionNode {
public:

	OperatorQuote(std::size_t limit, const Index<DefaultIndex>& index);
	OperatorQuote(const OperatorQuote& node);

	boost::optional<DocIdIterator> next(bool initialization=false) override;
	void quoteContinue() const;

private:
	DocIdIterator docIdBegin;
	PositionIterator positionBegin;
	mutable bool quoteBegin; 

	std::shared_ptr<QuoteBlock> quoteBlock;

	void resetLowerBound();
	void concreteInitializate() override;
};

class Leaf : public ExpressionNode {
public:
	Leaf(std::size_t hash, const Index<DefaultIndex>& index);
	boost::optional<DocIdIterator> next(bool initialization=false) override;
private:
	size_t offset;
	size_t length;
	size_t position;
};






