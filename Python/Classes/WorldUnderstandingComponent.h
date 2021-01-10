// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "MyEnumHelpers.h"
#include "WorldUnderstandingComponent.generated.h"


UENUM(Blueprintable)
enum class EAssociationType : uint8
{
	Emotional,
	Logical,


	COUNT UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct PYTHON_API FLabel
{
	GENERATED_BODY()

	FLabel()
	{}

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ObjectsType; // Which object class to search the World for

	UPROPERTY(EditAnywhere)
	uint8 IndexRelatedTo;

	//UPROPERTY(EditAnywhere)
	//	RelationType;
};

UENUM(Blueprintable)
enum class EStateEnumName : uint8
{
	ECharacterState,
	ECharacterStateChange,

	COUNT UMETA(Hidden),
};

UCLASS(EditInlineNew)
class PYTHON_API USentenceLabel : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Connections")
	FName ReferenceObjectName; // e.g. Whiterun_WhiteCat_03 // Within folder structure ../Whiterun/NPCs/

	UPROPERTY(EditAnywhere, Category = "Connections")
	EStateEnumName EnumName; // e.g. ECharacterState

	UPROPERTY()
	UEnum* SelectedEnumType;

	UPROPERTY(EditAnywhere, Category = "Connections")
	TArray<FDynamicEnumElement> EnumElements; // e.g. select ECharacterState::Playing

	UPROPERTY(EditAnywhere, Category = "Connections")
	FName ResponsableObjectName; // e.g. Whiterun_BallOfYarn_00 // Within folder structure None - obj too insignificant to have saved data

	/*UPROPERTY(EditAnywhere, Category = "Connections")
	UNormSituationalResponse* NormSituationalResponse;*/

protected:
	/*  */
	bool GetFloatByName(UObject * Target, FName VarName, float &outFloat);
};

UENUM(Blueprintable)
enum class ECalandarEntryType : uint8
{
	Holiday,
	Work,
	Parenting,


	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class EObjectFeature : uint8
{
	Object, // the object that contains the information
	ObjectDataType,
	ObjectLocation,
	ObjectRotation,
	ObjectScale,
	ObjectColour,
	LongTermMemory,
	WorkingMemory,// e.g. "how was your holiday?"
	ObjectCurrentHealth,

	COUNT UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct PYTHON_API FActorReference
{
	GENERATED_BODY()

	FActorReference()
		: Actor(nullptr)
		, bIsQuestion(true)
	{}

	/*FActorReference()
	{}*/

	UPROPERTY(EditAnywhere)
	AActor* Actor;

	UPROPERTY(EditAnywhere)
	EObjectFeature ObjectFeature;

	UPROPERTY(EditAnywhere)
	uint32 bIsQuestion : 1;
};

UENUM(Blueprintable)
enum class EActorType : uint8
{
	Wall,
	Car,


	Count UMETA(Hidden)
};
class UWorldStateComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PYTHON_API UWorldUnderstandingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldUnderstandingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool GetFeelingWell() const { return bFeelingWell; }
	void SetFeelingWell(bool isFeelingWell) { bFeelingWell = isFeelingWell; }

	/* Get the index of the actor that other functions will look at in roder to obtain the necessary information e.g. it's location. */
	bool GetReferredToObject(const TArray<FActorReference>& referencedActors, uint8 OUT_index);
	bool QuestionAsked(EObjectFeature infoTypeToRead, UWorldUnderstandingComponent* characterAsked);
	bool StatementReceived(EObjectFeature infoTypeToWrite, UWorldUnderstandingComponent* characterTold);

	// Memory start
	void UpdateDictionary();

	void AddToMemory(USentenceLabel* sentenceMeaning);
	bool GetMemoryFile(FString& OUT_filepath);
	// Memory end

	bool AskedIfFeelingWell(ACharacter* askingPerson)
	{
		// If decides not to share info
		if (false)
		{

		}

		// If has an emotional reaction to being asked to share this info
		if (false)
		{

		}

		return GetFeelingWell();
	}

	/* Returns whether the sentence receiver agreed/believed your statement - updated their fact dict with it's contents. */
	bool ToldIfFeelingWell(ACharacter* personMakingStatement)
	{

		//SetFeelingWell();

		return false;
	}

	// Find the object that the speaker was referning to in their sentence
	bool FindReferencedActor(const FString& actorType);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> ActorsThePlayerLookedAt;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> ImportantActors;

	UPROPERTY(EditAnywhere)
	TArray<FString> ReferencedActorNames; // Populated using words from sentences in specific indices

	UPROPERTY(EditAnywhere)
	TArray<FActorReference> ReferencedActors;
		
private:
	TArray<USentenceLabel*> WorkingMemory;
	const uint8 MaxNumWorkingMemoryChuncks = 4;
	const uint8 MaxNumWorkingMemories = 20;
	uint8 LatestWorkingMemoryIndex; // The most recent will be (LatestWorkingMemoryIndex - 1), and the next most-recent will be (LatestWorkingMemoryIndex - 2), and so on

	uint32 bFeelingWell : 1;

private:
	TArray<FName> ObjectNamingScheme{ TEXT("Location"),TEXT("ObjectName") ,TEXT("Instance") }; // e.g. Whiterun_WhiteCat_03 // Use object name (split index 1) to find object in world (use FString::Contains())
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PYTHON_API AWorldUnderstandingTester : public APawn
{
	GENERATED_BODY()

public:


private:
	UFUNCTION(Exec)
	void ec_SaySentence(FString sentence);
};
