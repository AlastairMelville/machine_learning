#pragma once

#include <memory>

#include "source/CoreObject/classes/Object/Object.h"
#include "source/Core/classes/Containers/Array.h"

class UWorld;
class UGameInstance;


struct FWorldContext
{
	/** Set CurrentWorld and update external reference pointers to reflect this*/
	void SetCurrentWorld(const std::shared_ptr<UWorld>& InWorld)
	{
		ThisCurrentWorld = InWorld;
	}

	std::shared_ptr<UWorld> World() const { return ThisCurrentWorld; }

	std::shared_ptr<UGameInstance> OwningGameInstance;

private:
	std::shared_ptr<UWorld> ThisCurrentWorld;
};

class UGameEngine : public UObject
{
public:
	UGameEngine()
	{}

	void Init();

	void Tick(float InDeltaTime);

	/**
	 * Kicks off a "Play From Here" request that was most likely made during a transaction
	 */
	void StartQueuedPlayMapRequest();

	void CreateGameInstance();

	FWorldContext& CreateNewWorldContext();

	FWorldContext* GetWorldContext();

public:
	std::shared_ptr<UGameInstance> GameInstance;

	std::shared_ptr<UWorld> World;

	//bool bIsPlayWorldQueued;

protected:
	TArray<FWorldContext*> WorldList;
};
