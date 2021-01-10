#pragma once

#include "source/CoreObject/classes/Object/ObjectGlobals.h"

class UObject
{
public:
	UObject()
		: Outer(nullptr)
	{}

	virtual ~UObject()
	{}

	UObject* Outer;
};

template<typename T = UObject>
std::unique_ptr<T> NewObject(T* ObjectPtr, UObject* InOuter)
{
	ObjectPtr->Outer = InOuter;
	return std::unique_ptr<T>(ObjectPtr);
}
