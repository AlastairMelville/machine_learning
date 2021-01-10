// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Tickable.h"
#include "LimitComputationInterface.generated.h"

struct PYTHON_API CalculationQueueEntry
{
	UObject* Requestor;
	TArray<float> CalculationMembers;

};

/* Singleton used to control the allowable machine learning computation. */
class PYTHON_API FComputationHandler : public FTickableGameObject
{
	friend class ILimitComputationInterface;

public:
	FComputationHandler();

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;

	virtual bool CanPerformNextCalculation() { return false; }

	virtual int32 GetNumCalculationsAllowedThisFrame() { return 0; }

	virtual int32 GetNumCalculationsAllowedThisSecond() { return 0; }

	int32 GetNumInQueue() { return CalculationQueue.Num(); }

protected:
	void AddToCalculationQueue(const CalculationQueueEntry& queueEntry);

	static TArray<CalculationQueueEntry> CalculationQueue;

	/*static int32 NumCurrentInstances;

	static int32 NumConnectedObjects;*/
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULimitComputationInterface : public UInterface
{
	GENERATED_BODY()
};

class PYTHON_API ILimitComputationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	enum
	{
		MAX_CALCULATIONS_PER_SECOND = 2,
	};

	ILimitComputationInterface();
	
	virtual bool CanPerformNextCalculation() { return false; }

	virtual int32 GetNumCalculationsAllowedThisFrame() { return 0; }

	virtual int32 GetNumCalculationsAllowedThisSecond() { return 0; }

	int32 GetNumInQueue();
};
