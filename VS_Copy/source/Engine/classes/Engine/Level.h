#pragma once

#include "source//CoreObject/classes/Object/Object.h"

class UWorld;
struct FLevelCollection;

class ULevel : UObject
{
public:
	UWorld* GetWorld() const
	{
		return OwningWorld;
	}

	/** Returns the cached collection that contains this level, if any. May be null. */
	FLevelCollection* GetCachedLevelCollection() const { return CachedLevelCollection; }

	/** Sets the cached level collection that contains this level. Should only be called by FLevelCollection. */
	void SetCachedLevelCollection(FLevelCollection* const InCachedLevelCollection) { CachedLevelCollection = InCachedLevelCollection; }

public:
	/** Data structures for holding the tick functions **/
	class FTickTaskLevel* TickTaskLevel;

private:
	UWorld* OwningWorld;

	/** Cached level collection that this level is contained in, for faster access than looping through the collections in the world. */
	FLevelCollection* CachedLevelCollection;
};
