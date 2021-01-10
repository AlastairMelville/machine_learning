// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WorldStateComponent.h"
#include "MyAIController.generated.h"

USTRUCT(BlueprintType)
struct PYTHON_API FCachedPerception
{
	GENERATED_BODY()

	FCachedPerception()
	{}

	FCachedPerception(APawn* character, ECharacterState percievedCharacterState, float time)
		: Character(character)
		, PercievedCharacterState(percievedCharacterState)
		, LastSighting(time)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState PercievedCharacterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastSighting;

	bool IsValid() { return Character; }
};

class UBlackboardData;
class AMyCharacter;

UCLASS()
class PYTHON_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMyAIController();

	virtual void Tick(float deltaTime) override;

	virtual void Possess(APawn* possessedPawn) override;

	virtual void UnPossess() override;

	UFUNCTION(BlueprintCallable)
	void UpdateCachedPerception(APawn* fuckYou, float timePerecieved, ECharacterState characterState = ECharacterState::CannotTell);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	AMyCharacter* PawnAsCharacter;

private:
	UPROPERTY(EditAnywhere)
	UBlackboardData* BlackboardAsset;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	TArray<FCachedPerception> CachedPerception;
};
