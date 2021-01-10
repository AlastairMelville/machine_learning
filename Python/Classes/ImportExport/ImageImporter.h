// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Engine/DataAsset.h"
#include "GameFramework/Actor.h"
#include "VictoryBPFunctionLibrary.h"
#include "ImageImporter.generated.h"


UENUM(Blueprintable)
enum class ETextImageLabel : uint8
{
	None,
	Hello,
	Bye,
	Cheese,
	Cows,

	Count UMETA(HIdden),
};

USTRUCT(BlueprintType)
struct PYTHON_API FPixelVectorLabelPair
{
	GENERATED_BODY()

	FPixelVectorLabelPair()
	{}

	FPixelVectorLabelPair(TArray<FLinearColor> pixels, ETextImageLabel label)
		: Pixels(pixels)
		, Label(label)
	{}

	UPROPERTY(VisibleAnywhere)
	TArray<FLinearColor> Pixels;

	UPROPERTY(VisibleAnywhere)
	ETextImageLabel Label;
};

class UTexture2D;

UCLASS()
class PYTHON_API AImageImporter : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	UTexture2D* ImportImagePixels(const FString& FullFilePath, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, 
		TArray<FLinearColor>& OutPixels, const FString& SaveFilePath, ETextImageLabel Label);

	UPROPERTY(EditAnywhere)
	FString Filepath;

	UPROPERTY(EditAnywhere)
	uint32 bImportImage : 1;

	UPROPERTY(EditAnywhere)
	TArray<FString> FeatureLabelArray;

	UPROPERTY(VisibleAnywhere)
	TArray<FPixelVectorLabelPair> PixelVectorLabelPairs;

	UFUNCTION(BlueprintCallable)
	bool GetRenderTargetPixels(UTexture* renderTexture);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture* RenderTargetTexture;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
