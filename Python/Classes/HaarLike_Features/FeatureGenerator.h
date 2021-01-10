// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FeatureGenerator.generated.h"


#if WITH_EDITOR
USTRUCT(BlueprintType)
struct PYTHON_API FPixelRow
{
	GENERATED_BODY()

	FPixelRow()
		: Pixels()
	{}

	UPROPERTY(EditAnywhere)
	TArray<float> Pixels;
};

UCLASS()
class PYTHON_API UFeatureGenerator : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFeatureGenerator();

private:
	UPROPERTY(EditAnywhere)
	TArray<FPixelRow> FeaturePixels;

	UPROPERTY(EditAnywhere)
	int32 RowLength;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
#endif // WITH_EDITOR
