#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "index.h"

using Iterator = Index<DefaultIndex>::DocIdIterator;
using SpecialIterator = IndexIterator<size_t, sizeof(size_t)>;
using DocId = Iterator::value_type;


class NotIteratorAdaptor {
public:

	NotIteratorAdaptor(size_t);
	~NotIteratorAdaptor();

	NotIteratorAdaptor& operator=(const boost::optional<Iterator>& iterator);
	NotIteratorAdaptor& operator++();
	SpecialIterator& operator*();

	operator boost::optional<Iterator>();
	operator bool();

private:
	boost::optional<SpecialIterator> currentEntry;
	size_t* docIds;
};



class ExpressionNode {
public:
	ExpressionNode();

	void initializate();
	virtual boost::optional<Iterator> current();
	virtual boost::optional<Iterator> next(bool initialization=false) = 0;


	std::unique_ptr<ExpressionNode> left;
	std::unique_ptr<ExpressionNode> right;

protected:
	boost::optional<Iterator> currentEntry;
private:
	virtual void concreteInitializate();
};

class OperatorAnd : public ExpressionNode {
public:
	OperatorAnd();
	boost::optional<Iterator> next(bool initialization=false) override;

private:
	void concreteInitializate() override;
};

class OperatorOr : public ExpressionNode {
public:
	OperatorOr();
	boost::optional<Iterator> next(bool initialization=false) override;

private:
	void concreteInitializate() override;
};

class OperatorNot: public ExpressionNode {
public:
	OperatorNot();
	boost::optional<Iterator> current() override;
	boost::optional<Iterator> next(bool initialization=false) override;

private:
	void concreteInitializate() override;

	size_t maxDocId;
	NotIteratorAdaptor specialCurrentEntry;
	boost::optional<size_t> boundaryDocId;
};

class Leaf : public ExpressionNode {
public:
	Leaf(size_t hash, const Index<DefaultIndex>& index);

	boost::optional<Iterator> next(bool initialization=false) override;
private:
	size_t offset;
	size_t length;
	size_t position;
};






