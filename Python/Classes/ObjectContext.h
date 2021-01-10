// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectContext.generated.h"


UCLASS()
class PYTHON_API UObjectContext : public UObject
{
	GENERATED_BODY()
	
public:	
};

UENUM(Blueprintable)
enum class ESentenceResultType : uint8
{
	Question, // Getter of information (could also set some info e.g. what the speaker could be curious about)
	Statement, // Setter of information
	COUNT UMETA(Hidden),
};

UENUM(Blueprintable, meta = (Bitflags))
enum class EEmotionalReactionOnPerceived : uint8
{
	Neutral,
	Fun,
	Surprise,
	Fear,
	Angeer,
	Sadness,
	Disgust,
	COUNT UMETA(Hidden),
};

/* Base information that applies to all world objects that is needed; but cannot be found in the AActor class etc. */
USTRUCT(BlueprintType)
struct PYTHON_API FBaseObjectSituation
{
	GENERATED_BODY()

	FBaseObjectSituation()
	{}

	FBaseObjectSituation(uint32 emotionalReactionsOnPerceived)
		: EmotionalReactionsOnPerceived(emotionalReactionsOnPerceived)
	{}

	UPROPERTY(EditAnywhere, meta = (Bitmask, BitmaskEnum = "EEmotionalReactionOnPerceived"))
	uint32 EmotionalReactionsOnPerceived;

	template<typename enumClass>
	static void SetBitmaskElements(enumClass& enumToChange, TArray<int32> enumIndices)
	{
		for (const int32& index : enumIndices)
		{
			enumToChange |= (1 << index);
		}
	}
};

UENUM(Blueprintable)
enum class EE : uint8
{
	COUNT UMETA(Hidden),
};

UENUM(Blueprintable)
enum class ERelationshipWithCharacter : uint8
{
	Stranger,
	Acquaintance,
	Friend,
	BestFriend,
	Family,
	COUNT UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct PYTHON_API FCharacterSituation : public FBaseObjectSituation
{
	GENERATED_BODY()

	FCharacterSituation()
		: CharacterNickname(TEXT(""))
	{}

	FCharacterSituation(FName characterNickname)
		: CharacterNickname(characterNickname)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterNickname;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)

};

struct PYTHON_API ObjectContextLink
{
	int32 ContextHierarchyLevel;

	/* Each level could be a group, or part of a group. */
	FName ContextHierarchyGroup;

	/* The situation that caused the link to this context. */
	FBaseObjectSituation RelatedSituation;

	/* A link to another context (may need to be an array). */
	ObjectContextLink* ContextLink;
};

struct PYTHON_API BaseObjectContext
{
	ObjectContextLink* ContextLink;
};
