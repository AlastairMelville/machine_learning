// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PythonCommunictionsActor.generated.h"


USTRUCT(BlueprintType)
struct PYTHON_API FIdentificationInfo
{
	GENERATED_BODY()

	FIdentificationInfo()
	{}

	FIdentificationInfo(const FString& person, const float& chanceOfError)
		: IdentifiedPerson(person)
		, ChanceOfError(chanceOfError)
	{}

	UPROPERTY(EditAnywhere)
	FString IdentifiedPerson;

	UPROPERTY(EditAnywhere)
	float ChanceOfError;
};

class UTextureRenderTarget2D;

UCLASS()
class PYTHON_API APythonCommunictionsActor : public AActor
{
	GENERATED_BODY()
	
public:
	APythonCommunictionsActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	static bool ExportFrameImage(const UTextureRenderTarget2D* renderTexture, const FVector2D imageDimensions, const FString suspectedPerson,
		const FString filepath);

	UFUNCTION(BlueprintCallable)
	static bool RetrieveObjLocsForFrameImage(const FString& filepath);

	UFUNCTION(BlueprintCallable)
	static bool MyStringToVector2DConv(const FString& IN_String, FVector2D& OUT_Vector);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTextureRenderTarget2D* RenderTexture;

	//UPROPERTY(EditAnywhere)
	static TArray<FString> FilesWaitingFor;

	UPROPERTY(EditAnywhere)
	TArray<FIdentificationInfo> IdentifiedPersons;

	UPROPERTY(EditAnywhere)
	FVector2D ImageDimensions;

	UPROPERTY(EditAnywhere)
	FString FullFilepath;

	//UPROPERTY(EditAnywhere)
	static int32 FrameNumber;

	UPROPERTY(EditAnywhere)
	FString SuspectedPerson = "Kirito";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FilepathExtension;

	TArray<FColor> Pixels;
	uint32 CurrentPixelNum;

	UPROPERTY(EditAnywhere)
	uint32 MaxPixelsToProcessPerSecond;

	UPROPERTY(EditAnywhere)
	FString ImageFilepath;

	UPROPERTY(EditAnywhere)
	FString FaceRecognitionImageFilepath;
	
private:

};
