#pragma once

#include <vector>

#include "source/Engine/classes/Engine/EngineBaseTypes.h"

class UWorld;
class ULevel;

/**
 * Interface for the tick task manager
 **/
class FTickTaskManagerInterface
{
public:
	virtual ~FTickTaskManagerInterface()
	{
	}

	/** Allocate a new ticking structure for a ULevel **/
	//virtual FTickTaskLevel* AllocateTickTaskLevel() = 0;

	/** Free a ticking structure for a ULevel **/
	//virtual void FreeTickTaskLevel(FTickTaskLevel* TickTaskLevel) = 0;

	/**
	 * Queue all of the ticks for a frame
	 *
	 * @param World	- World currently ticking
	 * @param DeltaSeconds - time in seconds since last tick
	 * @param TickType - type of tick (viewports only, time only, etc)
	 */
	virtual void StartFrame(UWorld* InWorld, float DeltaSeconds, /*ELevelTick TickType,*/ const std::vector<ULevel*>& LevelsToTick) = 0;

	/**
	 * Run all of the ticks for a pause frame synchronously on the game thread.
	 * The capability of pause ticks are very limited. There are no dependencies or ordering or tick groups.
	 * @param World	- World currently ticking
	 * @param DeltaSeconds - time in seconds since last tick
	 * @param TickType - type of tick (viewports only, time only, etc)
	 */
	virtual void RunPauseFrame(UWorld* InWorld, float DeltaSeconds, ELevelTick TickType, const std::vector<ULevel*>& LevelsToTick) = 0;

	/**
		* Run a tick group, ticking all actors and components
		* @param Group - Ticking group to run
		* @param bBlockTillComplete - if true, do not return until all ticks are complete
	*/
	virtual void RunTickGroup(ETickingGroup Group, bool bBlockTillComplete) = 0;

	/** Finish a frame of ticks **/
	virtual void EndFrame() = 0;

	/** Dumps all registered tick functions to output device. */
	//virtual void DumpAllTickFunctions(FOutputDevice& Ar, UWorld* InWorld, bool bEnabled, bool bDisabled, bool bGrouped) = 0;

	/** Returns a map of enabled ticks, grouped by 'diagnostic context' string, along with count of enabled ticks */
	//virtual void GetEnabledTickFunctionCounts(UWorld* InWorld, TSortedMap<FName, int32_t, FDefaultAllocator, FNameFastLess>& TickContextToCountMap, int32& EnabledCount, bool bDetailed) = 0;

	/**
	 * Singleton to retrieve the GLOBAL tick task manager
	 *
	 * @return Reference to the global cache tick task manager
	 */
	static FTickTaskManagerInterface& Get();
};
