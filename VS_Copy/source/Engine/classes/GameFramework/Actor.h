#pragma once

#include "source//CoreObject/classes/Object/Object.h"
#include "source/Engine/classes/Engine/EngineBaseTypes.h"

class AActor : public UObject
{
public:
	virtual void BeginPlay();

	virtual void Tick(float InDeltaTime);

	/**
	 * When called, will call the virtual call chain to register all of the tick functions for both the actor and optionally all components
	 * Do not override this function or make it virtual
	 * @param bRegister - true to register, false, to unregister
	 * @param bDoComponents - true to also apply the change to all components
	 */
	void RegisterAllActorTickFunctions(bool bRegister, bool bDoComponents);

	class ULevel* GetLevel() const;

protected:
	/**
	 * Virtual call chain to register all tick functions for the actor class hierarchy
	 * @param bRegister - true to register, false, to unregister
	 */
	virtual void RegisterActorTickFunctions(bool bRegister);

public:
	FActorTickFunction PrimaryActorTick;
};
