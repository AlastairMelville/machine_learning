// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WorldUnderstandingComponent.h"
#include "ObservedActorInfo.h"
#include "PythonCharacter.generated.h"


class APythonCharacter;
DECLARE_MULTICAST_DELEGATE_TwoParams(FChosenActionDelegate, EAction, APythonCharacter*)

UCLASS()
class PYTHON_API APythonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APythonCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	ECharacterState GetCharacterState() { return CharacterState; }

	ECharacterState PercieveCharacterState();

	/* Returns whether any of the observed actors were removed from the array */
	bool UpdateObservedActors();

	bool QueryObservedActor(const AActor* actor, const EObservedActorInfo& infoToQuery, bool& bFound, const FAIStimulus& infoToMatch);

	UFUNCTION(BlueprintCallable)
	void OnReceivedTargetUpdate(const FObservedActor& observedActorInfo);

	FName GetCharacterName() { return CharacterName; }

	// Get Character observable info Start
	EObservedEmotionalState GetObservableEmotionalState() { return ObservableEmotionalState; }


	// Get Character observable info End

	UFUNCTION(Exec)
	void cg_CharacterSayTheSentence(FString sentence);

	UFUNCTION(Exec)
	void cg_CharacterMakeDecision();

	FChosenActionDelegate ChosenActionDelegate;

	void OnObservedAction(EAction chosenAction, APythonCharacter* character);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FName CharacterName = TEXT("Dave");

	UPROPERTY(BlueprintReadWrite)
	TArray<FObservedActor> ObservedActors;

	UPROPERTY(EditAnywhere)
	float ObservationMaxAge;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	EAttentionLevel SpareAttentionLevel;

	EObservedEmotionalState ObservableEmotionalState;
	
private:
	ECharacterState CharacterState;
};
