// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "RL_Test_Pawn.h" // TEMP
#include "ObservedActorInfo.generated.h"


UENUM(Blueprintable)
enum class EObservedEmotionalState : uint8
{
	Unknown,
	Happy,
	Surprised,
	Sad,
	Angry,
	Disgusted,
	Fearful,

	Jealous,
	Bored,
	
	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EObservedActorInfo : uint8
{
	// Included in FAIStimulus //
	//Actor,
	Age,
	ExpirationAge,
	StimulusStrength,
	StimulusLocation,
	ReceiverLocation,
	StimulusTag,
	// Included in FAIStimulus //

	// Included in AActor //

	// Included in AActor //

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EAttentionLevel : uint8
{
	NotBusy,
	BitBusy,
	Busy,
	VeryBusy,
	NoSpareAttention,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EPersonalityTrait : uint8
{
	None,
	Emotionality,


	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct PYTHON_API FActionPersonalityTraitPair
{
	GENERATED_BODY()

	FActionPersonalityTraitPair()
	{}

	FActionPersonalityTraitPair(EAction action, EPersonalityTrait personalityTrait)
		: Action(action)
		, PersonalityTrait(personalityTrait)
	{}

	EAction Action;

	EPersonalityTrait PersonalityTrait;
};

USTRUCT(BlueprintType)
struct PYTHON_API FObservedPersonality
{
	GENERATED_BODY()

	FObservedPersonality()
		: ObservedPersonality()
	{
		ObservedPersonality.Add(EPersonalityTrait::Emotionality, 0);
		// TODO: Add remaining Personality Traits here
	}

	TMap<EPersonalityTrait, uint32> ObservedPersonality;
};

USTRUCT(BlueprintType)
struct PYTHON_API FObservedAction
{
	GENERATED_BODY()

	FObservedAction()
		: Action(EAction::Duck)
		, NumTimesChosen(0)
	{}

	FObservedAction(EAction action, uint32 num)
		: Action(action)
		, NumTimesChosen(num)
	{}

	UPROPERTY(EditAnywhere)
	EAction Action;

	UPROPERTY(EditAnywhere)
	uint32 NumTimesChosen;
};

USTRUCT(BlueprintType)
struct PYTHON_API FObservedActionContainer
{
	GENERATED_BODY()

	FObservedActionContainer()
		: ObservedActions()
	{
		//ObservedActions.SetNum(1);
	}

	FObservedActionContainer(EAction action)
		: ObservedActions()
	{
		ObservedActions.Add(FObservedAction(action, 0));
	}

	UPROPERTY(EditAnywhere)
	TArray<FObservedAction> ObservedActions;
};

USTRUCT(BlueprintType)
struct PYTHON_API FExistingObservedActorInfo
{
	GENERATED_BODY()

	FExistingObservedActorInfo()
	{}

	/* Holds the top e.g. 3 emotional states that the character is usually observed exhibiting. Each has a float value which says the proportion 
		of the time the character has been seen exhibiting this emotion in-front of the observing character. */
	UPROPERTY()
	TMap<EObservedEmotionalState, uint32 /*float*/> ObservedEmotionalStates = TMap<EObservedEmotionalState, uint32>{};
	uint32 NumEmotionalStateObservations = 0;

	/* Returns true if the Map element already existed. Returns false, if a new element was added*/
	bool AddToExistingEmotionalModel(const EObservedEmotionalState& observedEmotionalState);

	float CalculateChanceOfEmotion(const EObservedEmotionalState& potentialEmotionalState);


	UPROPERTY()
	TMap<FString, FObservedActionContainer> ObservedActions = TMap<FString, FObservedActionContainer>{};
	uint32 NumChosenActionsObserved = 0;
	FObservedPersonality ObservedPersonality = FObservedPersonality();

	bool AddToExistingActionModel(const FString& observedSituation, const EAction observedAction);

	float CalculateChanceOfAction(const FString& observedSituation, const EAction observedAction);

	bool UpdateObservedPersonality(const EAction observedAction);


	friend FORCEINLINE FArchive &operator << (FArchive &Ar, FExistingObservedActorInfo& existingObservedActorInfo)
	{
		Ar << existingObservedActorInfo.ObservedEmotionalStates;
		Ar << existingObservedActorInfo.NumEmotionalStateObservations;
		//Ar << existingObservedActorInfo.ObservedActions;
		//Ar << existingObservedActorInfo.NumChosenActionsObserved;

		return Ar;
	}
};

USTRUCT(BlueprintType)
struct PYTHON_API FObservedActor
{
	GENERATED_BODY()

	FObservedActor()
	{}

	FObservedActor(AActor* actor)
		: Actor(actor)
	{}

	UPROPERTY(BlueprintReadWrite)
	AActor* Actor;

	UPROPERTY(BlueprintReadWrite)
	FAIStimulus Stimulus;

	//UPROPERTY(EditAnywhere)
	FExistingObservedActorInfo ExistingObservedActorInfo;

	uint16 TotalDataNum = 0; // is float for calculating percentage

	EObservedEmotionalState ObservedEmotionalState;

	FString ActorName;

	/* Gets the percentage of actor data that you are already aware of. The more you already know, generally, the less you will subsequently notice. */
	uint16 GetPercentageUncovered();

	friend FORCEINLINE FArchive &operator << (FArchive &Ar, FObservedActor& observedActor)
	{
		Ar << observedActor.TotalDataNum;
		Ar << observedActor.ActorName;
		Ar << observedActor.ExistingObservedActorInfo;
		Ar << observedActor.ObservedEmotionalState;

		return Ar;
	}
};

UCLASS()
class PYTHON_API UObservedActorInfo : public UObject
{
	GENERATED_BODY()
	
public:

};
