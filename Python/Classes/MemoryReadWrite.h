// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryArchive.h"
#include "HAL/FileManager.h"
#if WITH_EDITOR
#include "GameFramework/Actor.h"
#endif // WITH_EDITOR

#include "RL_Test_Pawn.h" // TEMP
#include "GameFramework/Actor.h"
#include "ObservedActorInfo.h"

#include "MemoryReadWrite.generated.h"



/**
* Buffer archiver.
*/
class FMyBufferArchive : public FBufferArchive
{
public:
	void GetFileManager()
	{
		IFileManager& fileManager = IFileManager::Get();

		//GFileManager->
	}
};


/**
* Archive for reading arbitrary data from the specified memory location
*/
class FMyMemoryReader : public FMemoryArchive
{
public:

};

class UWorldStateComponent;
struct FTEST_Action;
struct FObservedActor;

UCLASS(EditInlineNew)
class PYTHON_API UMemoryReadWrite : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool SaveToFilePath(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor);

	UFUNCTION(BlueprintCallable)
	static bool LoadFromFilePath(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor);

	//FArchive is shared base class for FBufferArchive and FMemoryReader
	static void SaveLoadData(FArchive& Ar, TArray<FTEST_Action>& actions);

	static void SaveLoadData(FArchive& Ar, FObservedActor& observedActor);

	static bool SaveLoadWorldState(FArchive& Ar, const UWorldStateComponent* worldState);

	static bool SaveGameDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary, TArray<FTEST_Action>& actions, FObservedActor& observedActor);

	static bool LoadGameDataFromFile(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor);

	static bool SaveGameDataToFileCompressed(const FString& FullFilePath, int32& SaveDataInt32, FVector& SaveDataVector, TArray<FRotator>& SaveDataRotatorArray);

	static bool LoadGameDataFromFileCompressed(const FString& FullFilePath, int32& SaveDataInt32, FVector& SaveDataVector, TArray<FRotator>& SaveDataRotatorArray);

	template <typename EnumType>
	static FORCEINLINE EnumType GetEnumValueFromString(const FString& EnumName, const FString& String)
	{
		UEnum* Enum = FindObject<UEnum>((UObject*)ANY_PACKAGE, *EnumName, true);
		if (!Enum)
		{
			return EnumType(0);
		}
		return (EnumType)Enum->GetValueByName(FName(*String));
	}

	UPROPERTY(EditAnywhere)
	int32 NumGemsCollected;

	UPROPERTY(EditAnywhere)
	FVector PlayerLocation;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> ArrayOfRotationsOfTheStars;

	int32 NumGemsCollectedLoaded;

	FVector PlayerLocationLoaded;

	TArray<FRotator> ArrayOfRotationsOfTheStarsLoaded;

	friend FORCEINLINE FArchive &operator << (FArchive &Ar, AActor* actor)
	{
		FTransform transform = actor->GetTransform();
		Ar << transform;
		Ar << actor->Tags;

		return Ar;
	}

	
};

#if WITH_EDITOR
UCLASS()
class PYTHON_API AMemoryReadWriteTester : public AActor
{
	GENERATED_BODY()

public:
	AMemoryReadWriteTester();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UMemoryReadWrite* MemoryReaderWriter;
};
#endif // WITH_EDITOR
