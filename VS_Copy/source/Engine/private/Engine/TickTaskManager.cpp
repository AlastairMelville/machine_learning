
#include <pybind11/pybind11.h>

#include <set>

#include "source/Engine/classes/Engine/TickTaskManagerInterface.h"
#include "source/Engine/classes/Engine/EngineBaseTypes.h"
#include "source/Engine/classes/Engine/World.h"
#include "source/Engine/classes/Engine/Level.h"
#include "source/Core/classes/Async/TaskGraphInterfaces.h"

/**
 * Class that handles the actual tick tasks and starting and completing tick groups
 */
class FTickTaskSequencer
{
public:
	/**
	 * Class that handles dispatching a tick group
	 */
	class FDipatchTickGroupTask
	{
		/** Sequencer to proxy to **/
		FTickTaskSequencer& TTS;
		/** Tick group to dispatch **/
		ETickingGroup WorldTickGroup;
	public:
		/** Constructor
			* @param InTarget - Function to tick
			* @param InContext - context to tick in, here thread is desired execution thread
		**/
		__forceinline FDipatchTickGroupTask(FTickTaskSequencer& InTTS, ETickingGroup InWorldTickGroup)
			: TTS(InTTS)
			, WorldTickGroup(InWorldTickGroup)
		{
		}
	};

	/**
	 * Singleton to retrieve the global tick task sequencer
	 * @return Reference to the global tick task sequencer
	**/
	static FTickTaskSequencer& Get()
	{
		static FTickTaskSequencer SingletonInstance;
		return SingletonInstance;
	}

	void ReleaseTickGroup(ETickingGroup WorldTickGroup, bool bBlockTillComplete)
	{
		//FTaskGraphInterface::Get().WaitUntilTasksComplete(TickCompletionEvents[WorldTickGroup/*Block*/], ENamedThreads::GameThread);
	}

	/** Completion handles for each phase of ticks */
	std::vector<FGraphEvent> TickCompletionEvents[TG_MAX];
};

class FTickTaskLevel
{
public:
	/** Constructor, grabs, the sequencer singleton **/
	FTickTaskLevel()
		: TickTaskSequencer(FTickTaskSequencer::Get())
		, bTickNewlySpawned(false)
	{}

	/** Return true if this tick function is in the master list **/
	bool HasTickFunction(FTickFunction* TickFunction)
	{
		return true;// AllEnabledTickFunctions.Contains(TickFunction) || AllDisabledTickFunctions.Contains(TickFunction) || AllCoolingDownTickFunctions.Contains(TickFunction);
	}

	/** Add the tick function to the master list **/
	void AddTickFunction(FTickFunction* TickFunction)
	{
		if (TickFunction->TickState == FTickFunction::ETickState::Enabled)
		{
			AllEnabledTickFunctions.insert(TickFunction);
			if (bTickNewlySpawned)
			{
				NewlySpawnedTickFunctions.insert(TickFunction);
			}
		}
		else
		{
			AllDisabledTickFunctions.insert(TickFunction);
		}
	}

	/** Remove the tick function from the master list **/
	void RemoveTickFunction(FTickFunction* TickFunction)
	{

	}

	struct FCoolingDownTickFunctionList
	{
		FCoolingDownTickFunctionList()
			: Head(nullptr)
		{
		}

		bool Contains(FTickFunction* TickFunction) const
		{
			FTickFunction* Node = Head;
			while (Node)
			{
				if (Node == TickFunction)
				{
					return true;
				}
				Node = Node->Next;
			}
			return false;
		}

		FTickFunction* Head;
	};

	/** Global Sequencer */
	FTickTaskSequencer& TickTaskSequencer;

	/** true during the tick phase, when true, tick function adds also go to the newly spawned list. **/
	bool bTickNewlySpawned;

	/** Master list of enabled tick functions **/
	std::set<FTickFunction*> AllEnabledTickFunctions;
	/** Master list of enabled tick functions **/
	FCoolingDownTickFunctionList AllCoolingDownTickFunctions;
	/** Master list of disabled tick functions **/
	std::set<FTickFunction*> AllDisabledTickFunctions;
	/** List of tick functions added during a tick phase; these items are also duplicated in AllLiveTickFunctions for future frames **/
	std::set<FTickFunction*> NewlySpawnedTickFunctions;
};

/** Class that aggregates the individual levels and deals with parallel tick setup **/
class FTickTaskManager : public FTickTaskManagerInterface
{
public:
	/**
	 * Singleton to retrieve the global tick task manager
	 * @return Reference to the global tick task manager
	**/
	static FTickTaskManager& Get()
	{
		static FTickTaskManager SingletonInstance;
		return SingletonInstance;
	}

	/** Return true if this tick function is in the master list **/
	bool HasTickFunction(ULevel* InLevel, FTickFunction* TickFunction)
	{
		FTickTaskLevel* Level = TickTaskLevelForLevel(InLevel);
		return Level->HasTickFunction(TickFunction);
	}
	/** Add the tick function to the master list **/
	void AddTickFunction(ULevel* InLevel, FTickFunction* TickFunction)
	{
		//check(TickFunction->TickGroup >= 0 && TickFunction->TickGroup < TG_NewlySpawned); // You may not schedule a tick in the newly spawned group...they can only end up there if they are spawned late in a frame.
		FTickTaskLevel* Level = TickTaskLevelForLevel(InLevel);
		Level->AddTickFunction(TickFunction);
		TickFunction->TickTaskLevel = Level;
	}
	/** Remove the tick function from the master list **/
	void RemoveTickFunction(FTickFunction* TickFunction)
	{
		FTickTaskLevel* Level = TickFunction->TickTaskLevel;
		Level->RemoveTickFunction(TickFunction);
	}

	/** Find the tick level for this actor **/
	FTickTaskLevel* TickTaskLevelForLevel(ULevel* Level)
	{
		return Level->TickTaskLevel;
	}

	/**
	 * Queue all of the ticks for a frame
	 *
	 * @param World	- World currently ticking
	 * @param DeltaSeconds - time in seconds since last tick
	 * @param TickType - type of tick (viewports only, time only, etc)
	 */
	virtual void StartFrame(UWorld* InWorld, float DeltaSeconds, /*ELevelTick TickType,*/ const std::vector<ULevel*>& LevelsToTick) {}

	/**
	 * Run all of the ticks for a pause frame synchronously on the game thread.
	 * The capability of pause ticks are very limited. There are no dependencies or ordering or tick groups.
	 * @param World	- World currently ticking
	 * @param DeltaSeconds - time in seconds since last tick
	 * @param TickType - type of tick (viewports only, time only, etc)
	 */
	virtual void RunPauseFrame(UWorld* InWorld, float DeltaSeconds, ELevelTick TickType, const std::vector<ULevel*>& LevelsToTick) {}

	/**
		* Run a tick group, ticking all actors and components
		* @param Group - Ticking group to run
		* @param bBlockTillComplete - if true, do not return until all ticks are complete
	*/
	virtual void RunTickGroup(ETickingGroup Group, bool bBlockTillComplete);

	/** Finish a frame of ticks **/
	virtual void EndFrame() {}

private:
	/** Default constructor **/
	FTickTaskManager()
		: TickTaskSequencer(FTickTaskSequencer::Get())
		, bTickNewlySpawned(false)
	{}

public:

	/** Global Sequencer */
	FTickTaskSequencer& TickTaskSequencer;
	/** List of current levels **/
	std::vector<FTickTaskLevel*> LevelList;
	/** tick context **/
	//FTickContext Context;
	/** true during the tick phase, when true, tick function adds also go to the newly spawned list. **/
	bool bTickNewlySpawned;

	/** true during the tick phase, when true, tick function adds also go to the newly spawned list. **/
	std::vector<FTickFunction*> AllTickFunctions;
};

void FTickTaskManager::RunTickGroup(ETickingGroup Group, bool bBlockTillComplete)
{
	TickTaskSequencer.ReleaseTickGroup(Group, bBlockTillComplete);
}

void FTickFunction::RegisterTickFunction(class ULevel* Level)
{
	if (!bRegistered)
	{
		// Only allow registration of tick if we are are allowed on dedicated server, or we are not a dedicated server
		const UWorld* World = Level ? Level->GetWorld() : nullptr;
		if (World)
		{
			FTickTaskManager::Get().AddTickFunction(Level, this);
			bRegistered = true;
		}
	}
}

/**
 * Singleton to retrieve the global tick task manager
 * @return Reference to the global tick task manager
**/
FTickTaskManagerInterface& FTickTaskManagerInterface::Get()
{
	return FTickTaskManager::Get();
}



namespace py = pybind11;

void Init_TickTaskManagerInterface(py::module& m)
{
	
}
