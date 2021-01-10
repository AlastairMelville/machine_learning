// Fill out your copyright notice in the Description page of Project Settings.

#include "LimitComputationInterface.h"
#include "HAL/IConsoleManager.h"


static FComputationHandler* GComputationHandler = nullptr;

extern TAutoConsoleVariable<int32> CVarObjectRecognitionCalculationsPerSecond(
	TEXT("objectRecognition.ObjectREcognitionCalculationsPerSecond"),
	ILimitComputationInterface::MAX_CALCULATIONS_PER_SECOND,
	TEXT("Maximum number of object recognition that can occur per second"),
	ECVF_Default
);

static float TimeSinceLastCombine = 0.0f;

TArray<CalculationQueueEntry> FComputationHandler::CalculationQueue = TArray<CalculationQueueEntry>{};


// Add default functionality here for any ILimitComputationInterface functions that are not pure virtual.


FComputationHandler::FComputationHandler()
{

}

bool FComputationHandler::IsTickable() const
{
	return true;
}

TStatId FComputationHandler::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FCharacterFactoryMeshCombiner, STATGROUP_Tickables);
}

void FComputationHandler::AddToCalculationQueue(const CalculationQueueEntry& queueEntry)
{
	CalculationQueue.Add(queueEntry);
}

void FComputationHandler::Tick(float DeltaTime)
{
	TimeSinceLastCombine += DeltaTime;
	int32 maxCalculationsPerSecond;
#if UE_BUILD_SHIPPING
	maxCalculationsPerSecond = MAX_CALCULATIONS_PER_SECOND;
#else
	maxCalculationsPerSecond = CVarObjectRecognitionCalculationsPerSecond.GetValueOnGameThread();
#endif

	const float combineRate = 1.0f / maxCalculationsPerSecond;
	const uint32 numToSpawn = FMath::FloorToInt(TimeSinceLastCombine / combineRate);
	TimeSinceLastCombine = FMath::Max(0.f, (TimeSinceLastCombine - (numToSpawn * combineRate)));

	uint32 numProcessed = 0;
	while (numProcessed <= numToSpawn && GetNumInQueue() > 0)
	{
		for (int32 i = CalculationQueue.Num() - 1; i >= 0; --i)
		{


			CalculationQueue.RemoveAt(i);
		}
	}
}

ILimitComputationInterface::ILimitComputationInterface()
{

}

int32 ILimitComputationInterface::GetNumInQueue()
{
	if (!GComputationHandler)
	{
		GComputationHandler = new FComputationHandler;
	}

	if (GComputationHandler)
	{
		return GComputationHandler->GetNumInQueue();
	}

	return INDEX_NONE;
}
