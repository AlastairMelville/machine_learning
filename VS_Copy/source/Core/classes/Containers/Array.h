#pragma once

#include <vector>

template<typename T>
class TArray : public std::vector<T>
{
public:
	bool IsValidIndex(int32_t InIndex) const
	{
		return InIndex >= 0 && InIndex < size();
	}
};
