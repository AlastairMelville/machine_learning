#pragma once

/** Fake Thread safe counter, used to avoid cluttering code with ifdefs when counters are only used for debugging. */
class FNoopCounter
{
public:

	FNoopCounter() { }
	FNoopCounter(const FNoopCounter& Other) { }
	FNoopCounter(int32_t Value) { }

	int32_t Increment()
	{
		return 0;
	}

	int32_t Add(int32_t Amount)
	{
		return 0;
	}

	int32_t Decrement()
	{
		return 0;
	}

	int32_t Subtract(int32_t Amount)
	{
		return 0;
	}

	int32_t Set(int32_t Value)
	{
		return 0;
	}

	int32_t Reset()
	{
		return 0;
	}

	int32_t GetValue() const
	{
		return 0;
	}
};
