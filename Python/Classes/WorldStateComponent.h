// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WorldStateComponent.generated.h"


UENUM(Blueprintable)
enum class EEmotionalAssociation : uint8
{
	None,
	Fun,
	Surprise,
	Anger,
	Fear,
	Sadness,
	Disgust,
	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class ELogicalAssociation : uint8
{
	None,
	Similarity,
	Dissimilarity,
	IsPartOf,


	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class EFlowDirection : uint8
{
	LeftToRight,
	RightToLeft,

	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class ECharacterState : uint8
{
	CannotTell,
	Running,
	Socialising,


	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class ECharacterStateChange : uint8
{
	ChangedLocation,


	COUNT UMETA(Hidden),
};

/* Used to search the file correct directory for memories. */
UENUM(Blueprintable)
enum class ECalendarType : uint8
{
	Holiday,
	Work,
	OutOfOfficeHours,

	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class EPersonPhysicalStrength : uint8
{
	Strong,
	Weak,


	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EHeight : uint8
{
	Tall,
	Medium,
	Small,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EWeight : uint8
{
	Light,
	Medium,
	Heavy,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ESocialFeel : uint8
{
	Tense,
	Happy,


	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ERelationshipState : uint8
{
	Strong,
	Weak,
	BreakingUp,
	FallingOut,
	FriendshipToRomance,
	RomanceToFriendship,


	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ERelationshipType : uint8
{
	Acquaintance,
	Friend,
	BestFriend,
	Lover,
	Family,

	Count UMETA(Hidden)
};

class UFactDictionaryPair;
struct FRuntimeFloatCurve;

USTRUCT(BlueprintType)
struct PYTHON_API FAssociationMultipliers
{
	GENERATED_BODY()

	FAssociationMultipliers()
	{}

	UPROPERTY(EditAnywhere, Instanced)
	TArray<UFactDictionaryPair*> Features;

	UPROPERTY(EditAnywhere)
	TArray<FRuntimeFloatCurve> Multipliers; // The Feature (e.g. "ERelationshipType::Friend") will allow you to search for a value (e.g. 0.5f), which is then used as the Time in this curve

	float GetCurveValue(const int32& featureIndex);
};

USTRUCT(BlueprintType)
struct PYTHON_API FEmotionalAssociation
{
	GENERATED_BODY()

	FEmotionalAssociation()
	{}

	UPROPERTY(EditAnywhere)
	TArray<EEmotionalAssociation> EmotionalAssociation;

	UPROPERTY(EditAnywhere)
	TArray<FRuntimeFloatCurve> AssociatedEmotionalGraphs;
};

USTRUCT(BlueprintType)
struct PYTHON_API FEmotionalAssociationWithMultipliers : public FEmotionalAssociation
{
	GENERATED_BODY()

	FEmotionalAssociationWithMultipliers()
	{}

	UPROPERTY(EditAnywhere)
	TArray<FAssociationMultipliers> Multipliers; // e.g. if == ERelationship::Friend, then multiplier is 3.0f
};

UCLASS(EditInlineNew)
class PYTHON_API UNormSituationalResponse : public UDataAsset
{
	GENERATED_BODY()

public:
	/*UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float EmotionalToLocigalResponse;*/

	UPROPERTY(EditAnywhere)
	TArray<FEmotionalAssociation> BaseEmotionalAssociation; // TODO: Do same for Logical association

	UPROPERTY(EditAnywhere)
	TArray<FEmotionalAssociationWithMultipliers> ExtraEmotionalAssociation;

	/*UPROPERTY(EditAnywhere)
	TArray<ELogicalAssociation> LogicalAssociation;

	UPROPERTY(EditAnywhere)
	TArray<FRuntimeFloatCurve> AssociatedLogicalGraphs;*/ // How each logical association increases/decreases over time
};

UENUM(Blueprintable)
enum class ERanking : uint8
{
	Low,
	Medium,
	High,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EEmotion : uint8
{
	Fear,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EGoal : uint8
{
	MakeFriend,
	ExcelAtWork,

	Count UMETA(Hidden)
};

UCLASS()
class PYTHON_API UWorldStateComponent : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/*UPROPERTY(EditAnywhere)
	EGoal Goal;*/

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float PerceivedReward;

	UPROPERTY(EditAnywhere, Category = "Connections")
	UNormSituationalResponse* NormSituationalResponse;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UWorldStateComponent>> RelatedStates; // aka LogicalAssociation

	bool IsSomethingBool() const { return true; }

	uint8 IsSomething() const { return 0; }

	UPROPERTY(EditAnywhere)
	EPersonPhysicalStrength PersonPhysicalStrength;

	UPROPERTY(EditAnywhere)
	ESocialFeel SocialFeel;

	UPROPERTY(EditAnywhere)
	ERelationshipState RelationshipState;

	//UPROPERTY(EditAnywhere)


	//UPROPERTY(EditAnywhere)


	//UPROPERTY(EditAnywhere)

	
	//UPROPERTY(EditAnywhere)

	
	//UPROPERTY(EditAnywhere)

	
	//UPROPERTY(EditAnywhere)

	
	//UPROPERTY(EditAnywhere)

};

UCLASS()
class PYTHON_API UCharacterStateEntry : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint8 Hunger = 4;

	UPROPERTY(EditAnywhere)
	uint8 GroupSize = 1;

	UPROPERTY(EditAnywhere)
	uint8 Sleep = 5;
};
