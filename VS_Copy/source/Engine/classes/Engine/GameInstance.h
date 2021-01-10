#pragma once

#include <memory>

#include "source/CoreObject/classes/Object/Object.h"

class UGameInstance : public UObject
{
public:
	UGameInstance();

	void Initialise();

protected:
	std::shared_ptr<class UWorld> World;
};
