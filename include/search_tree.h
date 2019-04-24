#pragma once
#include <vector>
#include <string>


class SearchTree {
public:
	SearchTree() = default;
	~SearchTree() = default;

	template<typename T> void build(T&& expression);
private:
	template<typename T> T makeInverseExpression(T&& expression);
};



template<typename T>
void build(T&& expression) {

	decltype(auto) inverseExpression = makeInverseExpression(std::forward<T>(expression));

}

template<typename T>
T SearchTree::makeInverseExpression(T&& request) {

	return std::forward<T>(request);
}

