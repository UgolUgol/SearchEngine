#pragma once
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iterator>

template<typename NodeType, size_t NodeSize>
class IndexIterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = NodeType;
	using difference_type = size_t;
	using pointer = NodeType*;
	using reference = NodeType&;

	IndexIterator();
	IndexIterator(void* ptr);
	IndexIterator(NodeType* ptr);
	~IndexIterator() = default;

	IndexIterator& operator++();
	IndexIterator operator++(int);
	IndexIterator& operator--();
	IndexIterator operator--(int);

	IndexIterator operator+(size_t idx) const;
	IndexIterator operator-(size_t idx) const;

	const IndexIterator& operator+=(size_t idx);
	const IndexIterator& operator-=(size_t idx);
	const IndexIterator& operator=(const IndexIterator&);

	NodeType& operator*() const;
	NodeType& operator->() const;
	NodeType& operator[](size_t idx) const;

	friend bool operator==(const IndexIterator<NodeType, NodeSize>& lhs, const IndexIterator<NodeType, NodeSize>& rhs) {
		return lhs.ptr == rhs.ptr;
	}

	friend bool operator!=(const IndexIterator<NodeType, NodeSize>& lhs, const IndexIterator<NodeType, NodeSize>& rhs) {
		return !(lhs.ptr == rhs.ptr);
	}

	friend difference_type operator-(const IndexIterator<NodeType, NodeSize>& lhs, const IndexIterator<NodeType, NodeSize>& rhs) {

		return (lhs.ptr - rhs.ptr) / NodeSize;

	}
	
private:
	NodeType* ptr;
};


template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize>::IndexIterator() : ptr(nullptr) { }

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize>::IndexIterator(void* ptr) : ptr(reinterpret_cast<NodeType*>(ptr)) { }

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize>::IndexIterator(NodeType* ptr) : ptr(ptr) { }

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize>& IndexIterator<NodeType, NodeSize>::operator++() {
	
	this->ptr += NodeSize;
	return *this;

}

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize> IndexIterator<NodeType, NodeSize>::operator++(int){
	
	IndexIterator<NodeType, NodeSize> current_iterator(this->ptr);
	this->ptr += NodeSize;
	return current_iterator;

}

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize>& IndexIterator<NodeType, NodeSize>::operator--() {

	this->ptr -= NodeSize;
	return *this;

}

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize> IndexIterator<NodeType, NodeSize>::operator--(int) {

	IndexIterator<NodeType, NodeSize> current_iterator(this->ptr);
	this->ptr -= NodeSize;
	return current_iterator;
	
}

template<typename NodeType, size_t NodeSize>
const IndexIterator<NodeType, NodeSize>& IndexIterator<NodeType, NodeSize>::operator+=(size_t idx) {

	this->ptr += (idx * NodeSize);
	return *this;

}

template<typename NodeType, size_t NodeSize>
const IndexIterator<NodeType, NodeSize>& IndexIterator<NodeType, NodeSize>::operator-=(size_t idx) {

	this->ptr -= (idx * NodeSize);
	return *this;

}

template<typename NodeType, size_t NodeSize>
const IndexIterator<NodeType, NodeSize>& IndexIterator<NodeType, NodeSize>
::operator=(const IndexIterator<NodeType, NodeSize>& iterator) {

	this->ptr = iterator.ptr;
	return *this;

}

template<typename NodeType, size_t NodeSize>
NodeType& IndexIterator<NodeType, NodeSize>::operator*() const {
	return *(this->ptr);
}

template<typename NodeType, size_t NodeSize>
NodeType& IndexIterator<NodeType, NodeSize>::operator->() const {
	return *(this->ptr);
}

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize> IndexIterator<NodeType, NodeSize>::operator+(size_t idx) const {

	IndexIterator<NodeType, NodeSize> iterator(this->ptr);
	return iterator+=idx;

}

template<typename NodeType, size_t NodeSize>
IndexIterator<NodeType, NodeSize> IndexIterator<NodeType, NodeSize>::operator-(size_t idx) const {

	IndexIterator<NodeType, NodeSize> iterator(this->ptr);
	return iterator+=idx;

}

template<typename NodeType, size_t NodeSize>
NodeType& IndexIterator<NodeType, NodeSize>::operator[](size_t idx) const {
	return *(*this + idx);
}
