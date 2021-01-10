// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "IntegralImage.generated.h"


UCLASS()
class PYTHON_API AIntegralImage : public AActor
{
	GENERATED_BODY()
	
public:
	AIntegralImage();

	UFUNCTION(BlueprintCallable)
	void CreateIntegralImage(const TArray<float>& texturePixels, const int32 textureRowLength);
	
	UFUNCTION(BlueprintCallable)
	float CalculateIntegralBoxAverage(const uint8 rectangleSizeX, const uint8 rectangleSizeY, const uint8 startRow, const uint8 startCol, 
		const uint8 endRow, const uint8 endCol);

private:
	TArray<TArray<float>> IntegralImage;

	uint8 RowLength;
};
