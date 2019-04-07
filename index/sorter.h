#pragma once

template<typename InputType>
class Sorter {
public:
	using OutputType = InputType;
	OutputType sort(InputType& input);
};