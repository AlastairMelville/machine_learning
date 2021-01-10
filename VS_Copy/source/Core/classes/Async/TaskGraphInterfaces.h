#pragma once

#include <vector>

#include "source/Core/classes/Templates/RefCounting.h"

enum ENamedThreads
{
	GameThread,

	AnyThread,
};

/**
 *	A FGraphEvent is a list of tasks waiting for something.
 *	These tasks are call the subsequents.
 *	A graph event is a prerequisite for each of its subsequents.
 *	Graph events have a lifetime managed by reference counting.
 **/
class FGraphEvent
{
public:

};

/** Convenience typedef for a reference counted pointer to a graph event **/
typedef TRefCountPtr<class FGraphEvent> FGraphEventRef;

/** Convenience typedef for a an array a graph events **/
typedef std::vector<FGraphEvent> FGraphEventArray;

/** Interface tot he task graph system **/
class FTaskGraphInterface
{
	friend class FBaseGraphTask;

public:
	virtual ~FTaskGraphInterface()
	{
	}

	/**
	 *	Singleton for the system
	 *	@return a reference to the task graph system
	**/
	static FTaskGraphInterface& Get();

	/**
	 *	Requests that a named thread, which must be this thread, run until a list of tasks is complete.
	 *	@param	Tasks - tasks to wait for
	 *	@param	CurrentThread - This thread, must be a named thread
	**/
	virtual void WaitUntilTasksComplete(const FGraphEventArray& Tasks, ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread) = 0;
};

/**
 *	Base class for all tasks.
 *	Tasks go through a very specific life stage progression, and this is verified.
 **/
class FBaseGraphTask
{
public:

	// Allocator for small tasks.
	enum
	{
		/** Total size in bytes for a small task that will use the custom allocator **/
		SMALL_TASK_SIZE = 256
	};
	//typedef TLockFreeFixedSizeAllocator_TLSCache<SMALL_TASK_SIZE, PLATFORM_CACHE_LINE_SIZE> TSmallTaskAllocator;
protected:
	/**
	 *	Constructor
	 *	@param InNumberOfPrerequistitesOutstanding; the number of prerequisites outstanding. We actually add one to this to prevent the task from firing while we are setting up the task
	 **/
	FBaseGraphTask(int32_t InNumberOfPrerequistitesOutstanding)
		: ThreadToExecuteOn(ENamedThreads::AnyThread)
		, NumberOfPrerequistitesOutstanding(InNumberOfPrerequistitesOutstanding + 1) // + 1 is not a prerequisite, it is a lock to prevent it from executing while it is getting prerequisites, one it is safe to execute, call PrerequisitesComplete
	{
		//LLM(InheritedLLMTag = FLowLevelMemTracker::bIsDisabled ? ELLMTag::Untagged : (ELLMTag)FLowLevelMemTracker::Get().GetActiveTag(ELLMTracker::Default));
	}
	/**
	 *	Sets the desired execution thread. This is not part of the constructor because this information may not be known quite yet duiring construction.
	 *	@param InThreadToExecuteOn; the desired thread to execute on.
	 **/
	void SetThreadToExecuteOn(ENamedThreads InThreadToExecuteOn)
	{
		ThreadToExecuteOn = InThreadToExecuteOn;
	}

	/** Singleton to retrieve the small task allocator **/
	//static TSmallTaskAllocator& GetSmallTaskAllocator();

	/** destructor, just //checks the life stage **/
	virtual ~FBaseGraphTask()
	{
		////checkThreadGraph(LifeStage.Increment() == int32_t(LS_Deconstucted));
	}

private:
	//friend class FNamedTaskThread;
	//friend class FTaskThreadBase;
	//friend class FTaskThreadAnyThread;
	friend class FGraphEvent;
	friend class FTaskGraphImplementation;

	// Subclass API

	/**
	 *	Virtual call to actually execute the task. This should also call the destructor and free any memory.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 **/
	virtual void ExecuteTask(std::vector<FBaseGraphTask*>& NewTasks, ENamedThreads CurrentThread) = 0;

	// API called from other parts of the system

	/**
	 *	Called by the system to execute this task after it has been removed from an internal queue.
	 *	Just //checks the life stage and passes off to the virtual ExecuteTask method.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 **/
	__forceinline void Execute(std::vector<FBaseGraphTask*>& NewTasks, ENamedThreads CurrentThread)
	{
		ExecuteTask(NewTasks, CurrentThread);
	}

	// Internal Use

	/**
	 *	Queues the task for execution.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 **/
	void QueueTask(ENamedThreads CurrentThreadIfKnown)
	{
		//FTaskGraphInterface::Get().QueueTask(this, ThreadToExecuteOn, CurrentThreadIfKnown);
	}

	/**	Thread to execute on, can be ENamedThreads::AnyThread to execute on any unnamed thread **/
	ENamedThreads ThreadToExecuteOn;
	/**	Number of prerequisites outstanding. When this drops to zero, the thread is queued for execution.  **/
	/*FThreadSafeCounter*/int32_t NumberOfPrerequistitesOutstanding;


//#if !UE_BUILD_SHIPPING
//	// Life stage verification
//	// Tasks go through 8 steps, in order. In non-final builds, we track them with a thread safe counter and verify that the progression is correct.
//	enum ELifeStage
//	{
//		LS_BaseContructed = 0,
//		LS_Contructed,
//		LS_ThreadSet,
//		LS_PrequisitesSetup,
//		LS_Queued,
//		LS_Executing,
//		LS_Deconstucted,
//	};
//	FThreadSafeCounter			LifeStage;
//
//#endif

	//LLM(ELLMTag InheritedLLMTag);
};

/**
 *	TGraphTask
 *	Embeds a user defined task, as exemplified above, for doing the work and provides the functionality for setting up and handling prerequisites and subsequents
 **/
template<typename TTask>
class TGraphTask : public FBaseGraphTask
{
public:
	/**
	 *	This is a helper class returned from the factory. It constructs the embeded task with a set of arguments and sets the task up and makes it ready to execute.
	 *	The task may complete before these routines even return.
	 **/
	class FConstructor
	{
	public:
		/** Passthrough internal task constructor and dispatch. Note! Generally speaking references will not pass through; use pointers */
		template<typename...T>
		FGraphEventRef ConstructAndDispatchWhenReady(T&&... Args)
		{
			new ((void*)&Owner->TaskStorage) TTask(Forward<T>(Args)...);
			return Owner->Setup(Prerequisites, CurrentThreadIfKnown);
		}

		/** Passthrough internal task constructor and hold. */
		template<typename...T>
		TGraphTask* ConstructAndHold(T&&... Args)
		{
			new ((void*)&Owner->TaskStorage) TTask(Forward<T>(Args)...);
			return Owner->Hold(Prerequisites, CurrentThreadIfKnown);
		}

	private:
		friend class TGraphTask;

		/** The task that created me to assist with embeded task construction and preparation. **/
		TGraphTask* Owner;
		/** The list of prerequisites. **/
		const FGraphEventArray* Prerequisites;
		/** If known, the current thread.  ENamedThreads::AnyThread is also fine, and if that is the value, we will determine the current thread, as needed, via TLS. **/
		ENamedThreads CurrentThreadIfKnown;

		/** Constructor, simply saves off the arguments for later use after we actually construct the embeded task. **/
		FConstructor(TGraphTask* InOwner, const FGraphEventArray* InPrerequisites, ENamedThreads InCurrentThreadIfKnown)
			: Owner(InOwner)
			, Prerequisites(InPrerequisites)
			, CurrentThreadIfKnown(InCurrentThreadIfKnown)
		{
		}
		/** Prohibited copy construction **/
		FConstructor(const FConstructor& Other)
		{
			////check(0);
		}
		/** Prohibited copy **/
		void operator=(const FConstructor& Other)
		{
			////check(0);
		}
	};

	/**
	 *	Factory to create a task and return the helper object to construct the embedded task and set it up for execution.
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return a temporary helper class which can be used to complete the process.
	**/
	/*static FConstructor CreateTask(const FGraphEventArray* Prerequisites = NULL, ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		int32_t NumPrereq = Prerequisites ? Prerequisites->size() : 0;
		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			void* Mem = FBaseGraphTask::GetSmallTaskAllocator().Allocate();
			return FConstructor(new (Mem) TGraphTask(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget ? NULL : FGraphEvent::CreateGraphEvent(), NumPrereq), Prerequisites, CurrentThreadIfKnown);
		}
		return FConstructor(new TGraphTask(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget ? NULL : FGraphEvent::CreateGraphEvent(), NumPrereq), Prerequisites, CurrentThreadIfKnown);
	}*/

	void Unlock(ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		ConditionalQueueTask(CurrentThreadIfKnown);
	}

	/*FGraphEventRef GetCompletionEvent()
	{
		return Subsequents;
	}*/

private:
	friend class FConstructor;
	friend class FGraphEvent;

	// API derived from FBaseGraphTask

	/**
	 *	Virtual call to actually execute the task.
	 *	@param CurrentThread; provides the index of the thread we are running on. This is handy for submitting new taks.
	 *	Executes the embedded task.
	 *  Destroys the embedded task.
	 *	Dispatches the subsequents.
	 *	Destroys myself.
	 **/
	virtual void ExecuteTask(std::vector<FBaseGraphTask*>& NewTasks, ENamedThreads CurrentThread) final override
	{
		////checkThreadGraph(TaskConstructed);

		// Fire and forget mode must not have subsequents
		// Track subsequents mode must have subsequents
		////checkThreadGraph(XOR(TTask::GetSubsequentsMode() == ESubsequentsMode::FireAndForget, IsValidRef(Subsequents)));

		//if (TTask::GetSubsequentsMode() == ESubsequentsMode::TrackSubsequents)
		//{
		//	Subsequents->checkDontCompleteUntilIsEmpty(); // we can only add wait for tasks while executing the task
		//}

		//TTask& Task = *(TTask*)&TaskStorage;
		//{
		//	FScopeCycleCounter Scope(Task.GetStatId(), true);
		//	Task.DoTask(CurrentThread, Subsequents);
		//	Task.~TTask();
		//	////checkThreadGraph(ENamedThreads::GetThreadIndex(CurrentThread) <= ENamedThreads::GetRenderThread() || FMemStack::Get().IsEmpty()); // you must mark and pop memstacks if you use them in tasks! Named threads are excepted.
		//}

		TaskConstructed = false;

		/*if (TTask::GetSubsequentsMode() == ESubsequentsMode::TrackSubsequents)
		{
			FPlatformMisc::MemoryBarrier();
			Subsequents->DispatchSubsequents(NewTasks, CurrentThread);
		}*/

		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			this->TGraphTask::~TGraphTask();
			//FBaseGraphTask::GetSmallTaskAllocator().Free(this);
		}
		else
		{
			delete this;
		}
	}

	// Internals 

	/**
	 *	Private constructor, constructs the base class with the number of prerequisites.
	 *	@param InSubsequents subsequents to associate with this task. Thsi refernence is destroyed in the process!
	 *	@param NumberOfPrerequistitesOutstanding the number of prerequisites this task will have when it is built.
	**/
	/*TGraphTask(FGraphEventRef InSubsequents, int32_t NumberOfPrerequistitesOutstanding)
		: FBaseGraphTask(NumberOfPrerequistitesOutstanding)
		, TaskConstructed(false)
	{
		Subsequents.Swap(InSubsequents);
	}*/

	/**
	 *	Private destructor, just //checks that the task appears to be completed
	**/
	virtual ~TGraphTask() final override
	{
		//checkThreadGraph(!TaskConstructed);
	}

	/**
	 *	Call from FConstructor to complete the setup process
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@param bUnlock; if true, task can execute right now if possible
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	void SetupPrereqs(const FGraphEventArray* Prerequisites, ENamedThreads CurrentThreadIfKnown, bool bUnlock)
	{
		//checkThreadGraph(!TaskConstructed);
		TaskConstructed = true;
		TTask& Task = *(TTask*)&TaskStorage;
		SetThreadToExecuteOn(Task.GetDesiredThread());
		int32_t AlreadyCompletedPrerequisites = 0;
		if (Prerequisites)
		{
			for (int32_t Index = 0; Index < Prerequisites->Num(); Index++)
			{
				//check((*Prerequisites)[Index]);
				if (!(*Prerequisites)[Index]->AddSubsequent(this))
				{
					AlreadyCompletedPrerequisites++;
				}
			}
		}
		PrerequisitesComplete(CurrentThreadIfKnown, AlreadyCompletedPrerequisites, bUnlock);
	}

	/**
	 *	Call from FConstructor to complete the setup process
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return A new graph event which represents the completion of this task.
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	//FGraphEventRef Setup(const FGraphEventArray* Prerequisites = NULL, ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread)
	//{
	//	FGraphEventRef ReturnedEventRef = Subsequents; // very important so that this doesn't get destroyed before we return
	//	SetupPrereqs(Prerequisites, CurrentThreadIfKnown, true);
	//	return ReturnedEventRef;
	//}

	/**
	 *	Call from FConstructor to complete the setup process, but doesn't allow the task to dispatch yet
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to this task executing.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	 *	@return a pointer to the task
	 *
	 *	Create the completed event
	 *	Set the thread to execute on based on the embedded task
	 *	Attempt to add myself as a subsequent to each prerequisite
	 *	Tell the base task that I am ready to start as soon as my prerequisites are ready.
	 **/
	TGraphTask* Hold(const FGraphEventArray* Prerequisites = NULL, ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		SetupPrereqs(Prerequisites, CurrentThreadIfKnown, false);
		return this;
	}

	/**
	 *	Factory to create a gather task which assumes the given subsequent list from some other tasks.
	 *	This is used to support "WaitFor" during a task execution.
	 *	@param SubsequentsToAssume; subsequents to "assume" from an existing task
	 *	@param Prerequisites; the list of FGraphEvents that must be completed prior to dispatching my seubsequents.
	 *	@param CurrentThreadIfKnown; provides the index of the thread we are running on. Can be ENamedThreads::AnyThread if the current thread is unknown.
	**/
	/*static FConstructor CreateTask(FGraphEventRef SubsequentsToAssume, const FGraphEventArray* Prerequisites = NULL, ENamedThreads CurrentThreadIfKnown = ENamedThreads::AnyThread)
	{
		if (sizeof(TGraphTask) <= FBaseGraphTask::SMALL_TASK_SIZE)
		{
			void* Mem = FBaseGraphTask::GetSmallTaskAllocator().Allocate();
			return FConstructor(new (Mem) TGraphTask(SubsequentsToAssume, Prerequisites ? Prerequisites->Num() : 0), Prerequisites, CurrentThreadIfKnown);
		}
		return FConstructor(new TGraphTask(SubsequentsToAssume, Prerequisites ? Prerequisites->Num() : 0), Prerequisites, CurrentThreadIfKnown);
	}*/

	/** An aligned bit of storage to hold the embedded task **/
	//TAlignedBytes<sizeof(TTask), alignof(TTask)> TaskStorage;
	/** Used to sanity //check the state of the object **/
	bool TaskConstructed;
	/** A reference counted pointer to the completion event which lists the tasks that have me as a prerequisite. **/
	//FGraphEventRef Subsequents;
};

/**
 *	FReturnGraphTask is a task used to return flow control from a named thread back to the original caller of ProcessThreadUntilRequestReturn
 **/
class FReturnGraphTask
{
public:

	/**
	 *	Constructor
	 *	@param InThreadToReturnFrom; named thread to cause to return
	**/
	FReturnGraphTask(ENamedThreads InThreadToReturnFrom)
		: ThreadToReturnFrom(InThreadToReturnFrom)
	{
		//checkThreadGraph(ThreadToReturnFrom != ENamedThreads::AnyThread); // doesn't make any sense to return from any thread
	}

	/*__forceinline TStatId GetStatId() const
	{
		return GET_STATID(STAT_FReturnGraphTask);
	}*/

	/**
	 *	Retrieve the thread that this task wants to run on.
	 *	@return the thread that this task should run on.
	 **/
	ENamedThreads GetDesiredThread()
	{
		return ThreadToReturnFrom;
	}

	//static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }

	/**
	 *	Actually execute the task.
	 *	@param	CurrentThread; the thread we are running on
	 *	@param	MyCompletionGraphEvent; my completion event. Not always useful since at the end of DoWork, you can assume you are done and hence further tasks do not need you as a prerequisite.
	 *	However, MyCompletionGraphEvent can be useful for passing to other routines or when it is handy to set up subsequents before you actually do work.
	 **/
	//void DoTask(ENamedThreads CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	//{
	//	//checkThreadGraph(ENamedThreads::GetThreadIndex(ThreadToReturnFrom) == ENamedThreads::GetThreadIndex(CurrentThread)); // we somehow are executing on the wrong thread.
	//	FTaskGraphInterface::Get().RequestReturn(ThreadToReturnFrom);
	//}

private:
	/** Named thread that we want to cause to return to the caller of ProcessThreadUntilRequestReturn. **/
	ENamedThreads ThreadToReturnFrom;
};
