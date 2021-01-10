// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataAsset.h"
#include "DialogInformation.h"
#include "RL_Test_Pawn.generated.h"

UENUM(Blueprintable)
enum class EWeather : uint8
{
	Clear,
	MostlyClear,
	Overcast,
	Cloudy,
	BitRainy,
	Rainy,
	VeryRainy,
	Storm,
	

	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ETemperature : uint8
{
	VeryCold,
	Cold,
	Moderate,
	Warm,
	VeryWarm,
	Hot,


	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ETimeOfDay : uint8
{
	EarlyMorning,
	Morning,
	Noon,
	Afternoon,
	Evening,
	Night,


	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ETimeOfYear : uint8
{
	WinterEnd,
	SpringStart,
	Spring,
	SpringEnd,
	SummerStart,
	Summer,
	SummerEnd,
	AutumStart,
	Autum,
	AutumEnd,
	WinterStart,
	Winter,

	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ELighting : uint8
{
	VeryBright,
	Bright,
	Average,
	Dark,
	VeryDark,

	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EDistance : uint8
{
	AlreadyThere,
	VeryClose,
	Close,
	ModerateDistance,
	QuiteFar,
	Far,
	VeryFar,
	ExtremelyFar, // Like other country

	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EPersonSituationElement : uint8
{
	EClothing,
	EEmotionalState,
	EHeight,
	EWeight,
	EOccupation,


	COUNT UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EGlobalSituationElement : uint8
{
	/* A list of names of the enums are used to evaluate the global stuation/state. 
	   If someone asks "What is he weather like, you can get "Weather" as a string, add an "E" to the start are find the current value of your EWeather enum instance. */
	EWeather,
	ETemperature,
	ETimeOfDay,
	ETimeOfYear,
	ELighting,
	EDistance_FromHome,
	EDistance_FromWork,
	EDistance_FromNearestShelter,
	EDistance_FromNearestAffectableShelter,
	ERelationshipStatus,
	ERomanticRelationshipStatus,
	EEmotionalState_Yours, // 
	EEmotionalState_Other1, // 
	EEmotionalState_Other2, //
	EEmotionalState_Other3, //
	EDistance,



	COUNT UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct PYTHON_API FCurrentSituation
{
	GENERATED_BODY()

	FCurrentSituation()
	{}

	// String of characters from 0-9-Z (36 in totals). TODO: Make a TMap or TArray to map the characters to index numbers (e.g. 'A' = 10, 'Z' = 35).
	UPROPERTY(EditAnywhere)
	FString Situation = "504B310000";

	int64 GetEnumAtSituationIndex(const uint32& situationIndex);
};

UENUM(Blueprintable)
enum class EAction : uint8
{
	//Unknown,
	Duck,
	Jump,
	LieDown,

	Count UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ETest_Relationship : uint8
{
	DontCare,
	Friend,
	BestFriend,
	Dave, Fred, Cherry,
	Enemy,

	Count UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct PYTHON_API FTEST_DictionaryFact
{
	GENERATED_BODY()

	FTEST_DictionaryFact()
	{}

	FTEST_DictionaryFact(EKey key, ETest_Relationship value)
		: Key(key)
		, Value((int64)value)
	{}

	FTEST_DictionaryFact(EKey key, FString enumName, uint8 enumElementIdx)
		: Key(key)
		, EnumName(enumName)
		, EnumElementIdx(enumElementIdx)
	{}

	UPROPERTY(EditAnywhere)
	EKey Key;

	//UPROPERTY(EditAnywhere)
	int64 Value; // Converted enum element value

	UPROPERTY(EditAnywhere)
	FString EnumName;

	UPROPERTY(EditAnywhere)
	uint8 EnumElementIdx;

	friend FORCEINLINE FArchive &operator << (FArchive &Ar, FTEST_DictionaryFact& fact)
	{
		int64 convKey = (int64)fact.Key;
		Ar << fact.Key;
		Ar << fact.EnumName;
		Ar << fact.EnumElementIdx;

		return Ar;
	}
};

/* How does each action change the situation */
USTRUCT(BlueprintType)
struct PYTHON_API FActionSituationPair
{
	GENERATED_BODY()

	FActionSituationPair()
	{}

	UPROPERTY(EditAnywhere)
	EAction Action;

	/* Ignore string indices with a value of 0 */
	UPROPERTY(EditAnywhere)
	FString SituationChanges;
};

USTRUCT(BlueprintType)
struct PYTHON_API FTEST_Action
{
	GENERATED_BODY()

	FTEST_Action()
	{}

	FTEST_Action(EAction action, float perceivedReward)
		: Action(action)
		, PerceivedReward(perceivedReward)
		, Reasons() // TDOD: Make a param passed in
	{}

	UPROPERTY(EditAnywhere)
	EAction Action; // TODO: could replace with enum name as string and enum element index to make serialisation easier?

	UPROPERTY(EditAnywhere)
	float PerceivedReward;

	UPROPERTY(EditAnywhere)
	TArray<FString> Reasons;

	/* You only think that this score will be accurate (e.g. "Next time I will tell him how I feel"), rather than learned the score via experience. */
	uint32 bPredictedReward : 1;

	bool operator < (const FTEST_Action& other) const
	{
		return other.PerceivedReward < PerceivedReward;
	}

	friend FORCEINLINE FArchive &operator << (FArchive &Ar, FTEST_Action& action)
	{
		int64 convKey = (int64)action.Action;
		Ar << convKey;
		Ar << action.PerceivedReward;

		return Ar;
	}
};

class UFactDictionaryPair;

UCLASS(EditInlineNew)
class PYTHON_API UTEST_State : public UDataAsset
{
	GENERATED_BODY()

public:
	/* If the relevant dictionary(ies) is in this state, then the weights for the attached Actions apply to the Agent's next decision. */
	UPROPERTY(EditAnywhere)
	TArray<FTEST_DictionaryFact> DictionaryState;

	UPROPERTY(EditAnywhere)
	TArray<FTEST_Action> Actions;

#if WITH_EDITOR
	UPROPERTY(EditAnywhere)
	uint8 bMakeManualSave : 1;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	friend FORCEINLINE FArchive &operator << (FArchive &Ar, UTEST_State& state)
	{
		Ar << state.DictionaryState;
		Ar << state.Actions;

		return Ar;
	}
};

USTRUCT(BlueprintType)
struct PYTHON_API FStatePtrPair
{
	GENERATED_BODY()

	FStatePtrPair()
	{}

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTEST_State> StatePtr;

	UTEST_State* State;

	UTEST_State* Get()
	{
		if (!State)
		{
			State = StatePtr.LoadSynchronous();
		}

		return State;
	}
};

UCLASS()
class PYTHON_API ARL_Test_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARL_Test_Pawn();

	static bool GetMemoryFilePath(FString& filepath, const TArray<FTEST_DictionaryFact>& dictionaryFacts);

	static EAction ChooseAction();
	  	
public:
	static TArray<ETest_Relationship> PairToLookFor;
	
	static UTEST_State* FoundState;

	UPROPERTY(EditAnywhere/*, Instanced*/)
	TArray<FStatePtrPair> StatePtrPairs;

	UFUNCTION(Exec)
	static EAction cg_SayTheSentence(FString sentence);

	UFUNCTION(Exec)
	static void RespondToAction(float score);
};

static TArray<FTEST_DictionaryFact> TEST_StaticDictionary = TArray<FTEST_DictionaryFact>
{
	// FORMAT: (Key, EnumName, EnumValue)
	FTEST_DictionaryFact(EKey::Speaker, "ETest_Relationship", 1),
	FTEST_DictionaryFact(EKey::Listener, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Listener_b, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Listener_c, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Health, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Ammo, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::LastSeenTime, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Weapon, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::EnemyCount, "ETest_Relationship", 0),
	FTEST_DictionaryFact(EKey::Count, ETest_Relationship::DontCare) UMETA(Hidden),
};
