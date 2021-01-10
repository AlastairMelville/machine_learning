#pragma once

#include <vector>

enum ETickingGroup
{
	/** Any item that can be run in parallel with our physics simulation work. */
	TG_DuringPhysics,

	TG_MAX,
};

/** Type of tick we wish to perform on the level */
enum ELevelTick
{
	/** Update the level time only. */
	LEVELTICK_TimeOnly = 0,
	/** Update time and viewports. */
	LEVELTICK_ViewportsOnly = 1,
	/** Update all. */
	LEVELTICK_All = 2,
	/** Delta time is zero, we are paused. Components don't tick. */
	LEVELTICK_PauseTick = 3,
};

/**
* Abstract Base class for all tick functions.
**/
struct FTickFunction
{
	/**
	 * Defines the minimum tick group for this tick function. These groups determine the relative order of when objects tick during a frame update.
	 * Given prerequisites, the tick may be delayed.
	 *
	 * @see ETickingGroup
	 * @see FTickFunction::AddPrerequisite()
	 */
	enum ETickingGroup TickGroup;

	/**
	 * Defines the tick group that this tick function must finish in. These groups determine the relative order of when objects tick during a frame update.
	 *
	 * @see ETickingGroup
	 */
	enum ETickingGroup EndTickGroup;

protected:
	/** Internal data that indicates the tick group we actually started in (it may have been delayed due to prerequisites) **/
	enum ETickingGroup ActualStartTickGroup;

	/** Internal data that indicates the tick group we actually started in (it may have been delayed due to prerequisites) **/
	enum ETickingGroup ActualEndTickGroup;

public:
	/** Bool indicating that this function should execute even if the game is paused. Pause ticks are very limited in capabilities. **/
	uint8_t bTickEvenWhenPaused : 1;

	/** If false, this tick function will never be registered and will never tick. Only settable in defaults. */
	uint8_t bCanEverTick : 1;

	/** If true, this tick function will start enabled, but can be disabled later on. */
	uint8_t bStartWithTickEnabled : 1;

	/** If we allow this tick to run on a dedicated server */
	uint8_t bAllowTickOnDedicatedServer : 1;

	/** Run this tick first within the tick group, presumably to start async tasks that must be completed with this tick group, hiding the latency. */
	uint8_t bHighPriority : 1;

	/** If false, this tick will run on the game thread, otherwise it will run on any thread in parallel with the game thread and in parallel with other "async ticks" **/
	uint8_t bRunOnAnyThread : 1;

private:
	/** If true, means that this tick function is in the master array of tick functions */
	bool bRegistered;

	/** Cache whether this function was rescheduled as an interval function during StartParallel */
	bool bWasInterval;

	enum class ETickState : uint8_t
	{
		Disabled,
		Enabled,
		CoolingDown
	};

	/**
	 * If Disabled, this tick will not fire
	 * If CoolingDown, this tick has an interval frequency that is being adhered to currently
	 * CAUTION: Do not set this directly
	 **/
	ETickState TickState;

	/** Internal data to track if we have started visiting this tick function yet this frame **/
	int32_t TickVisitedGFrameCounter;

	/** Internal data to track if we have finshed visiting this tick function yet this frame **/
	int32_t TickQueuedGFrameCounter;

	/** Pointer to the task, only used during setup. This is often stale. **/
	void* TaskPointer;

	/** Prerequisites for this tick function **/
	std::vector<struct FTickPrerequisite> Prerequisites;

	/** The next function in the cooling down list for ticks with an interval*/
	FTickFunction* Next;

	/**
	  * If TickFrequency is greater than 0 and tick state is CoolingDown, this is the time,
	  * relative to the element ahead of it in the cooling down list, remaining until the next time this function will tick
	  */
	float RelativeTickCooldown;

	/**
	* The last world game time at which we were ticked. Game time used is dependent on bTickEvenWhenPaused
	* Valid only if we've been ticked at least once since having a tick interval; otherwise set to -1.f
	*/
	float LastTickGameTimeSeconds;

public:

	/** The frequency in seconds at which this tick function will be executed.  If less than or equal to 0 then it will tick every frame */
	float TickInterval;

	/** Back pointer to the FTickTaskLevel containing this tick function if it is registered **/
	class FTickTaskLevel* TickTaskLevel;

	/** Default constructor, intitalizes to reasonable defaults **/
	FTickFunction();
	/** Destructor, unregisters the tick function **/
	virtual ~FTickFunction();

	/**
	 * Adds the tick function to the master list of tick functions.
	 * @param Level - level to place this tick function in
	 **/
	void RegisterTickFunction(class ULevel* Level);
	/** Removes the tick function from the master list of tick functions. **/
	//void UnRegisterTickFunction();
	/** See if the tick function is currently registered */
	//bool IsTickFunctionRegistered() const { return bRegistered; }

	/** Enables or disables this tick function. **/
	//void SetTickFunctionEnable(bool bInEnabled);
	/** Returns whether the tick function is currently enabled */
	//bool IsTickFunctionEnabled() const { return TickState != ETickState::Disabled; }
	/** Returns whether it is valid to access this tick function's completion handle */
	//bool IsCompletionHandleValid() const { return TaskPointer != nullptr; }
	/**
	* Gets the current completion handle of this tick function, so it can be delayed until a later point when some additional
	* tasks have been completed.  Only valid after TG_PreAsyncWork has started and then only until the TickFunction finishes
	* execution
	**/
	//FGraphEventRef GetCompletionHandle() const;

	/**
	* Gets the action tick group that this function will be elligible to start in.
	* Only valid after TG_PreAsyncWork has started through the end of the frame.
	**/
	enum ETickingGroup GetActualTickGroup() const
	{
		return ActualStartTickGroup;
	}

	/**
	* Gets the action tick group that this function will be required to end in.
	* Only valid after TG_PreAsyncWork has started through the end of the frame.
	**/
	enum ETickingGroup GetActualEndTickGroup() const
	{
		return ActualEndTickGroup;
	}


	/**
	 * Adds a tick function to the list of prerequisites...in other words, adds the requirement that TargetTickFunction is called before this tick function is
	 * @param TargetObject - UObject containing this tick function. Only used to verify that the other pointer is still usable
	 * @param TargetTickFunction - Actual tick function to use as a prerequisite
	 **/
	//void AddPrerequisite(UObject* TargetObject, struct FTickFunction& TargetTickFunction);
	/**
	 * Removes a prerequisite that was previously added.
	 * @param TargetObject - UObject containing this tick function. Only used to verify that the other pointer is still usable
	 * @param TargetTickFunction - Actual tick function to use as a prerequisite
	 **/
	//void RemovePrerequisite(UObject* TargetObject, struct FTickFunction& TargetTickFunction);
	/**
	 * Sets this function to hipri and all prerequisites recursively
	 * @param bInHighPriority - priority to set
	 **/
	//void SetPriorityIncludingPrerequisites(bool bInHighPriority);

	/**
	 * @return a reference to prerequisites for this tick function.
	 */
	std::vector<struct FTickPrerequisite>& GetPrerequisites()
	{
		return Prerequisites;
	}

	/**
	 * @return a reference to prerequisites for this tick function (const).
	 */
	const std::vector<struct FTickPrerequisite>& GetPrerequisites() const
	{
		return Prerequisites;
	}

private:
	/**
	 * Queues a tick function for execution from the game thread
	 * @param TickContext - context to tick in
	 */
	//void QueueTickFunction(class FTickTaskSequencer& TTS, const FTickContext& TickContext);

	/**
	 * Queues a tick function for execution from the game thread
	 * @param TickContext - context to tick in
	 * @param StackForCycleDetection - Stack For Cycle Detection
	 */
	//void QueueTickFunctionParallel(const FTickContext& TickContext, TArray<FTickFunction*, TInlineAllocator<8> >& StackForCycleDetection);

	/** Returns the delta time to use when ticking this function given the TickContext */
	//float CalculateDeltaTime(const FTickContext& TickContext);

	/**
	 * Logs the prerequisites
	 */
	//void ShowPrerequistes(int32_t Indent = 1);

	/**
	 * Abstract function actually execute the tick.
	 * @param DeltaTime - frame time to advance, in seconds
	 * @param TickType - kind of tick for this frame
	 * @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
	 * @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completetion of this task until certain child tasks are complete.
	 **/
	//virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	//{
	//	check(0); // you cannot make this pure virtual in script because it wants to create constructors.
	//}
	///** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
	//virtual FString DiagnosticMessage()
	//{
	//	check(0); // you cannot make this pure virtual in script because it wants to create constructors.
	//	return FString(TEXT("invalid"));
	//}
	///** Function to give a 'context' for this tick, used for grouped active tick reporting */
	//virtual FName DiagnosticContext(bool bDetailed)
	//{
	//	return NAME_None;
	//}

	friend class FTickTaskSequencer;
	friend class FTickTaskManager;
	friend class FTickTaskLevel;
	//friend class FTickFunctionTask;

	// It is unsafe to copy FTickFunctions and any subclasses of FTickFunction should specify the type trait WithCopy = false
	FTickFunction& operator=(const FTickFunction&) = delete;
};

/**
* Tick function that calls AActor::TickActor
**/
struct FActorTickFunction : public FTickFunction
{

	/**  AActor  that is the target of this tick **/
	class AActor* Target;

	/**
		* Abstract function actually execute the tick.
		* @param DeltaTime - frame time to advance, in seconds
		* @param TickType - kind of tick for this frame
		* @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
		* @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completetion of this task until certain child tasks are complete.
	**/
	//virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
	//virtual FString DiagnosticMessage() override;
	//virtual FName DiagnosticContext(bool bDetailed) override;
};
