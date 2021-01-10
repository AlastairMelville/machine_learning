
#include "source/Engine/classes/Engine/EngineTypes.h"
#include "source/Engine/classes/Engine/Level.h"
#include "source/Engine/classes/Engine/World.h"

#include <algorithm>
#include <pybind11/pybind11.h>

FLevelCollection::FLevelCollection() = default;

FLevelCollection::~FLevelCollection()
{
	for (ULevel* Level : Levels)
	{
		if (Level)
		{
			Level->SetCachedLevelCollection(nullptr);
		}
	}
}

FLevelCollection::FLevelCollection(FLevelCollection&& Other)
	: Levels(std::forward< std::set<ULevel*> >(Other.Levels))
{
	for (ULevel* Level : Levels)
	{
		if (Level)
		{
			Level->SetCachedLevelCollection(this);
		}
	}
}

FLevelCollection& FLevelCollection::operator=(FLevelCollection&& Other)
{
	if (this != &Other)
	{
		for (ULevel* Level : Levels)
		{
			if (Level)
			{
				Level->SetCachedLevelCollection(this);
			}
		}
	}

	return *this;
}

void FLevelCollection::SetPersistentLevel(const std::shared_ptr<ULevel>& Level)
{
	PersistentLevel = Level;
}

void FLevelCollection::AddLevel(const std::shared_ptr<ULevel>& Level)
{
	if (Level)
	{
		// Sanity check that Level isn't already in another collection.
		Levels.insert(Level.get());
		Level->SetCachedLevelCollection(this);
	}
}

void FLevelCollection::RemoveLevel(const std::shared_ptr<ULevel>& Level)
{
	if (Level)
	{
		Level->SetCachedLevelCollection(nullptr);
		Levels.erase(Level.get());
	}
}

void UWorld::InitWorld()
{
	ConditionallyCreateDefaultLevelCollections();
}

std::shared_ptr<UWorld> UWorld::CreateWorld()
{
	return std::make_shared<UWorld>();
}

const FLevelCollection* UWorld::GetActiveLevelCollection() const
{
	if (LevelCollections.IsValidIndex(ActiveLevelCollectionIndex))
	{
		return &LevelCollections[ActiveLevelCollectionIndex];
	}

	return nullptr;
}

void UWorld::SetActiveLevelCollection(int32_t LevelCollectionIndex)
{
	ActiveLevelCollectionIndex = LevelCollectionIndex;
	const FLevelCollection* const ActiveLevelCollection = GetActiveLevelCollection();

	if (ActiveLevelCollection == nullptr)
	{
		return;
	}

	PersistentLevel = ActiveLevelCollection->GetPersistentLevel();
}

FLevelCollection& UWorld::FindOrAddCollectionByType(const ELevelCollectionType InType)
{
	for (FLevelCollection& LC : LevelCollections)
	{
		if (LC.GetType() == InType)
		{
			return LC;
		}
	}

	// Not found, add a new one.
	FLevelCollection NewLC;
	NewLC.SetType(InType);
	LevelCollections.push_back(std::forward<FLevelCollection>(NewLC));
	return LevelCollections.back();
}

int32_t UWorld::FindOrAddCollectionByType_Index(const ELevelCollectionType InType)
{
	const int32_t FoundIndex = FindCollectionIndexByType(InType);

	if (FoundIndex != INDEX_NONE)
	{
		return FoundIndex;
	}

	// Not found, add a new one.
	FLevelCollection NewLC;
	NewLC.SetType(InType);
	LevelCollections.push_back(std::forward<FLevelCollection>(NewLC));
	return static_cast<int32_t>(LevelCollections.size()) - 1;
}

FLevelCollection* UWorld::FindCollectionByType(const ELevelCollectionType InType)
{
	for (FLevelCollection& LC : LevelCollections)
	{
		if (LC.GetType() == InType)
		{
			return &LC;
		}
	}

	return nullptr;
}

const FLevelCollection* UWorld::FindCollectionByType(const ELevelCollectionType InType) const
{
	for (const FLevelCollection& LC : LevelCollections)
	{
		if (LC.GetType() == InType)
		{
			return &LC;
		}
	}

	return nullptr;
}

int32_t UWorld::FindCollectionIndexByType(const ELevelCollectionType InType) const
{
	std::vector<FLevelCollection>::const_iterator itr = std::find_if(LevelCollections.begin(), LevelCollections.end(),
		[InType] (const FLevelCollection& Item) { return Item.GetType() == InType; });

	return static_cast<int32_t>(itr - LevelCollections.begin());
}

void UWorld::ConditionallyCreateDefaultLevelCollections()
{
	LevelCollections.reserve((int32_t)ELevelCollectionType::MAX);

	// Create main level collection. The persistent level will always be considered dynamic.
	if (!FindCollectionByType(ELevelCollectionType::DynamicSourceLevels))
	{
		// Default to the dynamic source collection
		ActiveLevelCollectionIndex = FindOrAddCollectionByType_Index(ELevelCollectionType::DynamicSourceLevels);
		LevelCollections[ActiveLevelCollectionIndex].SetPersistentLevel(PersistentLevel);

		// Don't add the persistent level if it is already a member of another collection.
		// This may be the case if, for example, this world is the outer of a streaming level,
		// in which case the persistent level may be in one of the collections in the streaming level's OwningWorld.
		if (PersistentLevel->GetCachedLevelCollection() == nullptr)
		{
			LevelCollections[ActiveLevelCollectionIndex].AddLevel(PersistentLevel);
		}
	}

	if (!FindCollectionByType(ELevelCollectionType::StaticLevels))
	{
		FLevelCollection& StaticCollection = FindOrAddCollectionByType(ELevelCollectionType::StaticLevels);
		StaticCollection.SetPersistentLevel(PersistentLevel);
	}
}

void UWorld::Tick(float InDeltaTime)
{
	for (int32_t i = 0; i < LevelCollections.size(); ++i)
	{
		// Build a list of levels from the collection that are also in the world's Levels array.
		// Collections may contain levels that aren't loaded in the world at the moment.
		std::vector<ULevel*> LevelsToTick;
		for (ULevel* CollectionLevel : LevelCollections[i].GetLevels())
		{
			if (std::find(Levels.begin(), Levels.end(), CollectionLevel) != Levels.end())
			{
				LevelsToTick.push_back(CollectionLevel);
			}
		}

		FTickTaskManagerInterface::Get().StartFrame(this, InDeltaTime, LevelsToTick);

		RunTickGroup(TG_DuringPhysics);
	}
}

/**
	* Run a tick group, ticking all actors and components
	* @param Group - Ticking group to run
	* @param bBlockTillComplete - if true, do not return until all ticks are complete
	*/
void UWorld::RunTickGroup(ETickingGroup Group, bool bBlockTillComplete/* = true */)
{
	//check(TickGroup == Group); // this should already be at the correct value, but we want to make sure things are happening in the right order
	FTickTaskManagerInterface::Get().RunTickGroup(Group, bBlockTillComplete);
	//TickGroup = ETickingGroup(TickGroup + 1); // new actors go into the next tick group because this one is already gone
}



namespace py = pybind11;

void Init_World(py::module& m)
{
	/*py::class_<UWorld>(m, "UWorld")
		.def(py::init<>())
		.def_static("CreateWorld", &UWorld::CreateWorld);*/
}
