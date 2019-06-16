#pragma once

#include <cstddef>
#include <vector>


namespace Varcode {

constexpr unsigned char blockSize = 128;

template<typename T>
std::vector<unsigned char> encodeNumber(T element) 
{

	std::vector<unsigned char> bytes;
	while(true) {

		bytes.insert(std::begin(bytes), element % blockSize);
		if(element < blockSize) {
			
			break;
		
		}

		element /= blockSize;
	}

	bytes[bytes.size() - 1] += blockSize;
	return bytes;
}
	
template<typename T>
std::vector<unsigned char> encode(const std::vector<T>& elements)
{

	std::vector<unsigned char> byteStream;
	for(auto& element : elements) {

		auto bytes = encodeNumber(element);
		byteStream.insert(std::end(byteStream), std::begin(bytes), std::end(bytes));

	}

	return byteStream;
}


template<typename T>
std::vector<T> decode(const unsigned char* byteStream, std::size_t size)
{

	std::vector<T> elements;
	T element = 0;
	
	auto begin = byteStream;
	while(byteStream < begin + size) {

		if(*byteStream < blockSize) {

			element = blockSize * element + *byteStream;

		} else {

			element = blockSize * element + (*byteStream - blockSize);
			elements.push_back(element);
			element = 0;

		}	
		++byteStream;

	}

	return elements;
}


template<typename Iterator>
void findDifference(Iterator begin, Iterator end, std::size_t step)
{
	
	size_t accumulateCount = 0;
	for(auto current = begin + step + 1; current < end; std::advance(current, step + 1)) {

		*current -= (*begin + accumulateCount);
		accumulateCount += *current;

	}


}

template<typename Iterator>
void restoreDifference(Iterator begin, Iterator end, std::size_t step)
{
	size_t accumulateCount = 0;
	for(auto current = begin + step + 1; current < end; std::advance(current, step + 1)) {

		accumulateCount += *current;
		*current = (*begin + accumulateCount);

	}	

}

template<typename T>
std::vector<unsigned char> compress(std::vector<T>& elements)
{

	findDifference(std::begin(elements), std::end(elements), 1);
	return encode(elements);

}

template<typename T>
std::vector<T> decompress(const unsigned char* byteStream, std::size_t size) 
{

	auto elements = decode<T>(byteStream, size);
	restoreDifference(std::begin(elements), std::end(elements), 1);
	
	return elements;

}

}