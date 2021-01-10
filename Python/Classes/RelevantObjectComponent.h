// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RelevantObjectComponent.generated.h"


UENUM(Blueprintable)
enum class EObjectState : uint8
{
	Stored,
	Abondoned,
	

	COUNT UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct PYTHON_API FCharacterObjectState
{
	GENERATED_BODY()

	FCharacterObjectState()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APawn* RelatedPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectState ObjectState;
};

UCLASS()
class PYTHON_API URelevantObjectComponent : public UObject
{
	GENERATED_BODY()

public:
	
private:
	/* The state that each interacting character has most recently left the object in (e.g. you have left your phone to the side while you exercise). */
	TArray<FCharacterObjectState> PawnRelatedObjectStates;
};
