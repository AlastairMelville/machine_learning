#pragma once

#include <set>

#include "source/Engine/classes/Engine/TickTaskManagerInterface.h"
#include "source/Core/classes/CoreTypes.h"
#include "source/Core/classes/Containers/Array.h"


enum ELevelCollectionType;

/**
 * Contains a group of levels of a particular ELevelCollectionType within a UWorld
 * and the context required to properly tick/update those levels. This object is move-only.
 */
struct FLevelCollection
{
	FLevelCollection();

	/** The destructor will clear the cached collection pointers in this collection's levels. */
	~FLevelCollection();

	FLevelCollection(const FLevelCollection&) = default;
	FLevelCollection& operator=(const FLevelCollection&) = default;

	FLevelCollection(FLevelCollection&& Other);
	FLevelCollection& operator=(FLevelCollection&& Other);

	/** Gets the type of this collection. */
	ELevelCollectionType GetType() const { return CollectionType; }

	/** Sets the type of this collection. */
	void SetType(const ELevelCollectionType InType) { CollectionType = InType; }

	/** Returns the set of levels in this collection. */
	const std::set<ULevel*>& GetLevels() const { return Levels; }

	/** Sets this collection's PersistentLevel and adds it to the Levels set. */
	void SetPersistentLevel(const std::shared_ptr<ULevel>& Level);

	/** Returns this collection's PersistentLevel. */
	std::shared_ptr<ULevel> GetPersistentLevel() const { return PersistentLevel; }

	/** Adds a level to this collection and caches the collection pointer on the level for fast access. */
	void AddLevel(const std::shared_ptr<ULevel>& Level);

	/** Removes a level from this collection and clears the cached collection pointer on the level. */
	void RemoveLevel(const std::shared_ptr<ULevel>& Level);

private:
	/**
	 * The persistent level associated with this collection.
	 * The source collection and the duplicated collection will have their own instances.
	 */
	std::shared_ptr<ULevel> PersistentLevel;

	/** All the levels in this collection. */
	std::set<ULevel*> Levels;

	/** The type of this collection. */
	ELevelCollectionType CollectionType;
};

class UWorld
{
public:
	UWorld()
		: TickGroup(ETickingGroup::TG_DuringPhysics)
		, PersistentLevel(nullptr)
		, ActiveLevelCollectionIndex(INDEX_NONE)
	{}

	void InitWorld();

	static std::shared_ptr<UWorld> CreateWorld();

	/**
	 * Returns the level collection which currently has its context set on this world. May be null.
	 * If non-null, this implies that execution is currently within the scope of an FScopedLevelCollectionContextSwitch for this world.
	 */
	const FLevelCollection* GetActiveLevelCollection() const;

	/**
	 * Returns the index of the level collection which currently has its context set on this world. May be INDEX_NONE.
	 * If not INDEX_NONE, this implies that execution is currently within the scope of an FScopedLevelCollectionContextSwitch for this world.
	 */
	int32_t GetActiveLevelCollectionIndex() const { return ActiveLevelCollectionIndex; }

	/** Sets the level collection and its context on this world. Should only be called by FScopedLevelCollectionContextSwitch. */
	void SetActiveLevelCollection(int32_t LevelCollectionIndex);

	FLevelCollection& FindOrAddCollectionByType(const ELevelCollectionType InType);

	int32_t FindOrAddCollectionByType_Index(const ELevelCollectionType InType);

	FLevelCollection* FindCollectionByType(const ELevelCollectionType InType);

	const FLevelCollection* FindCollectionByType(const ELevelCollectionType InType) const;

	int32_t FindCollectionIndexByType(const ELevelCollectionType InType) const;

	void ConditionallyCreateDefaultLevelCollections();

	void Tick(float InDeltaTime);

	void RunTickGroup(ETickingGroup Group, bool bBlockTillComplete = true);

public:
	/** The current ticking group																								*/
	ETickingGroup TickGroup;

	/** Persistent level containing the world info, default brush and actors spawned during gameplay among other things			*/
	std::shared_ptr<ULevel> PersistentLevel;

	/** Array of levels currently in this world. Not serialized to disk to avoid hard references.								*/
	std::vector<ULevel*> Levels;

	/** Array of level collections currently in this world. */
	TArray<FLevelCollection> LevelCollections;

	/** Index of the level collection that's currently ticking. */
	int32_t ActiveLevelCollectionIndex;
};
