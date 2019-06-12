#pragma once

#include <cstddef>
#include <vector>


namespace Varcode {

constexpr unsigned blockSize = 128;

template<typename T>
std::vector<unsigned char> encodeNumber(T element) 
{

	std::vector<unsigned char> bytes;
	while(true) {

		bytes.push_back(element % blockSize);
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
std::vector<T> decode(const std::vector<unsigned char>& byteStream)
{

	std::vector<T> elements;
	T element = 0;
	for(auto& byte : byteStream) {

		if(byte < blockSize) {

			element = blockSize * element + byte;

		} else {

			element = blockSize * element + (byte - blockSize);
			elements.push_back(element);
			element = 0;

		}	
	}

	return elements;
}

}