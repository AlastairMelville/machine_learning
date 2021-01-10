// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "UObject/NoExportTypes.h"
#include "Misc/AutomationTest.h"
#include "GameFramework/Actor.h"
#include "PerformanceCommon.generated.h"


UCLASS()
class PYTHON_API UPerformanceCommon : public UObject
{
	GENERATED_BODY()
	
public:
	
};

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitCommand, double, Duration);



//struct FFlyToInfo
//{
//	FFlyToInfo(float speed, float arrivalThreshold, const FVector& target, const FRotator& targetRot)
//		: Speed(speed)
//		, SqArrivalThreshold(arrivalThreshold * arrivalThreshold)
//		, TargetLocation(target)
//		, TargetRotation(targetRot)
//	{
//	}
//
//	float Speed;
//	float SqArrivalThreshold;
//
//	FVector TargetLocation;
//	FRotator TargetRotation;
//};
//
//// 
//// Struct with information about a location at which we want to perform a test, including which map it belongs to
////
//struct FLocationInfo
//{
//	FLocationInfo(const FString& mapName, AActor* locationActor, FRotator rotationOffset, bool spawnCharacter)
//	{
//		MapName = mapName;
//		Location = locationActor->GetActorLocation();
//		Name = locationActor->GetName();
//		Rotation = locationActor->GetActorRotation() + rotationOffset;
//		SpawnCharacter = spawnCharacter;
//	}
//
//	FString MapName;
//	FString Name;
//	FVector Location;
//
//	FRotator Rotation;
//
//	bool SpawnCharacter;
//};
//
//
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FExecConsoleCommand, FString, Command);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FTravelCommand, FString, Map);
//DEFINE_LATENT_AUTOMATION_COMMAND(FWaitForMapLoadCommand);
//DEFINE_LATENT_AUTOMATION_COMMAND(FBeginSpectateCommand);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FBeginPlayCommand, class AAnglingCharacter*, Character);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FFlyToCommand, FFlyToInfo, FlyToInfo);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FSetPlayerPositionCommand, FVector, Location);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FSetPlayerRotationCommand, FRotator, Rotation);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FWaitCommand, double, Duration);
//DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FSetGraphicsQualitySettingsCommand, int32, QualityLevel);
//DEFINE_LATENT_AUTOMATION_COMMAND(FDisableAutomaticTooltipsCommand);
//
//static FString FishingTestsDateFormat = FString(TEXT("%d%m%y-%H%M%S"));
//static const float FlyToSpeedUnitsPerSecond = 4000.0f;
//
//class FFishingTestGlobals
//{
//public:
//	static FFishingTestGlobals* Get()
//	{
//		if (Instance == nullptr)
//		{
//			Instance = new FFishingTestGlobals();
//		}
//
//		return Instance;
//	}
//
//	const FString& GetDateFormat() const
//	{
//		return FishingTestsDateFormat;
//	}
//
//	const FString& GetTestId() const
//	{
//		return TestId;
//	}
//
//	const FString& GetTestDataPath() const
//	{
//		return DataPath;
//	}
//
//	float GetTestTargetFPS() const
//	{
//		return TargetFPS;
//	}
//
//	void SetTestId(const FString& testID)
//	{
//		TestId = testID;
//	}
//
//	void SetTestDataPath(const FString& dataPath)
//	{
//		DataPath = dataPath;
//	}
//
//	void SetTestTargetFPS(float fps)
//	{
//		TargetFPS = fps;
//	}
//
//	FString GetMetaDataString(FDateTime* overrideDateTime = nullptr) const;
//
//private:
//	FString FishingTestsDateFormat = FString(TEXT("%d%m%y-%H%M%S"));
//	FString TestId = TEXT("");
//	FString DataPath = TEXT("");
//	float TargetFPS;
//
//	static FFishingTestGlobals* Instance;
//};
//
//namespace NFishingTestHelpers
//{
//	class UWorld* GetWorld();
//	class AAnglingCharacter* GetCharacter(class UWorld* world);
//	class AAnglingPlayerController* GetController(class UWorld* world);
//
//	FString GetMapNameFromParameter(const FString& mapParam);
//	FString GetMapPathFromParameter(const FString& mapParam);
//
//
//	void GetMapTests(TArray<FString>& outBeautifiedNames, TArray<FString>& outTestCommands);
//
//};
//
//class FRandomInputCommand : public IAutomationLatentCommand
//{
//public:
//	FRandomInputCommand(double duration, const TArray<FName>& actionNames)
//		: ActionIndex(-1)
//		, KeyIndex(-1)
//		, Pressed(false)
//		, Duration(duration)
//	{
//		ActionNames = actionNames;
//	}
//
//protected:
//
//	int32 ActionIndex;
//	int32 KeyIndex;
//	bool Pressed;
//
//	double Duration;
//
//	TArray<FName> ActionNames;
//};
//
//class FRandomAxisInputCommand : public FRandomInputCommand
//{
//public:
//	FRandomAxisInputCommand(double duration, const TArray<FName>& actionNames);
//
//	virtual bool Update() override;
//};
//
//class FRandomKeyInputCommand : public FRandomInputCommand
//{
//public:
//	FRandomKeyInputCommand(float duration, const TArray<FName>& actionNames);
//
//	virtual bool Update() override;
//};
//
//
//class FRodMovementCommand : public IAutomationLatentCommand
//{
//public:
//	FRodMovementCommand(double duration, float fixedAxisValue);
//
//	virtual bool Update() override;
//
//private:
//	double Duration;
//	float AxisValue;
//};
//
//class FSpawnCharacterCommand : public IAutomationLatentCommand
//{
//public:
//	FSpawnCharacterCommand()
//		: RequestedSpawn(false)
//		, ChallengeStarted(false)
//	{
//	}
//
//	virtual bool Update() override;
//
//private:
//	bool RequestedSpawn;
//	bool ChallengeStarted;
//};
//
//class FSetTimeOfDayCommand : public IAutomationLatentCommand
//{
//public:
//	FSetTimeOfDayCommand(int32 hour)
//		: Hour(hour)
//		, FirstUpdate(true)
//	{
//	}
//
//	virtual bool Update() override;
//
//private:
//	int32 Hour;
//	bool FirstUpdate;
//};
//
//class FTestLocationHelper
//{
//public:
//	void GatherLocations(const FString& mapName);
//
//	const TArray<FLocationInfo>& GetLocations() const { return Locations; }
//
//private:
//	float GetTotalFlythroughDistance();
//	void SortLocations(FVector refPos);
//
//	TArray<FLocationInfo> Locations;
//};
