// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WordSimilarity.generated.h"


UCLASS()
class PYTHON_API UWordSimilarity : public UObject
{
	GENERATED_BODY()

public:

};

UENUM(Blueprintable)
enum class EFormattingType : uint8
{
	Default,
	Sentences,
	WordList,
	Numbers,
	Count UMETA(Hidden)
};

UCLASS()
class PYTHON_API UWordToVecFormatting : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		uint32 bFormatText : 1;

	UPROPERTY(EditAnywhere)
		uint32 bFormatWordToVecData : 1;

#if WITH_EDITOR
	/* Useed to convert sentences with punctuation etc. into text that an be used for a WordToVec models creation. */
	static void FormatText(const FString& fromFilename, const FString& toFilename);

	static void FormatWordToVecData(const FString& fromFilename, const FString& toFilename);

	UPROPERTY(EditAnywhere, Category = "Formatting")
		EFormattingType FormattingType;

	static bool GetFormattedData(const FString& fromFilename, TArray<FDoubleArray>& OUT_vectorValues, TArray<FString>& OUT_wordData, 
		EFormattingType overrideFormattingType = EFormattingType::Default);

	// Testing start
	bool TestEditorOnly_GetFormattedData(const FString& fromFilename, EFormattingType overrideFormattingType = EFormattingType::Default);

	UPROPERTY(EditAnywhere, Category = "Testing")
		uint32 bTestGetFormattedData : 1;

	UPROPERTY(EditAnywhere, Category = "Testing")
		int32 GrabIndex;

	UPROPERTY(EditAnywhere, Category = "Testing")
		FString FromFileName;

	UPROPERTY(EditAnywhere, Category = "Testing")
		FString ToFileName;

	UPROPERTY(EditAnywhere, Category = "Testing|Words")
		TArray<FString> WordData;

	UPROPERTY(EditAnywhere, Category = "Testing|WordVectors")
		TArray<FStringArray> VectorValuesAsStrings;

	UPROPERTY(EditAnywhere, Category = "Testing|WordVectors")
		TArray<FDoubleArray> VectorValues;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};

USTRUCT(BlueprintType)
struct PYTHON_API FTextSwitch
{
	GENERATED_BODY()

		FTextSwitch()
		: Remove("")
		, Replacement("")
	{}

	FTextSwitch(FString remove, FString replacement)
		: Remove(remove)
		, Replacement(replacement)
	{}

	UPROPERTY(EditAnywhere)
		FString Remove;

	UPROPERTY(EditAnywhere)
		FString Replacement;
};

USTRUCT(BlueprintType)
struct PYTHON_API FSavedTextSwitch
{
	GENERATED_BODY()

		FSavedTextSwitch()
		: SavedTextSwitches()
	{}

	UPROPERTY(EditAnywhere)
		FName TextSwitchName;

	UPROPERTY(EditAnywhere)
		TArray<FTextSwitch> SavedTextSwitches;
};

UCLASS()
class PYTHON_API UCustomTextFormatting : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FString LocationFilename;

	UPROPERTY(EditAnywhere)
		FString DestinationFilename;

	UPROPERTY(EditAnywhere)
		uint32 bFormat : 1;

	UPROPERTY(EditAnywhere)
		TArray<FTextSwitch> TextSwitches;

	UPROPERTY(EditAnywhere)
		TArray<FSavedTextSwitch> SavedTextSwitches;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
