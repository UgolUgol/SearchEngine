#pragma once
#include <set>
#include <stack>
#include <memory>
#include <boost/optional.hpp>
#include "index.h"

using Iterator = Index<DefaultIndex>::DocIdIterator;
using DocId = Iterator::value_type;


class NotIteratorAdaptor {
public:
	NotIteratorAdaptor();

	NotIteratorAdaptor& operator=(const boost::optional<Iterator>& iterator);
	NotIteratorAdaptor& operator++();
	Iterator& operator*();

	operator boost::optional<Iterator>();
	operator bool();

private:
	boost::optional<Iterator> currentEntry;
	size_t docId;
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

	NotIteratorAdaptor specialCurrentEntry;
	boost::optional<size_t> boundaryDocId;
	size_t maxDocId;
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






