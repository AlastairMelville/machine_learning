// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectDetectionPrediction.generated.h"

struct FDoubleArray;

UCLASS()
class PYTHON_API AObjectDetectionPrediction : public AActor
{
	GENERATED_BODY()
	
public:
	AObjectDetectionPrediction();

	virtual void Tick(float DeltaTime) override;

	float MaxPool(TArray<FDoubleArray> pixelMatrix);

	TArray<FDoubleArray> CalculateConvolutionalLayer(const TArray<FDoubleArray>& imagePixelValues, const TArray<FDoubleArray>& imagePixelWeights, const FVector2D& dims);

	TArray<FDoubleArray> GetPixelValuesAndWeights(const TArray<FDoubleArray>& imagePixelValues, const TArray<FDoubleArray>& imagePixelWeights,
		const FVector2D& startingPixelLoc, const FVector2D& dims, TArray<FDoubleArray>& OUT_pixelValues, TArray<FDoubleArray>& OUT_pixelWeights);

	double CalculateConvolutionalLayerElement(const TArray<FDoubleArray>& originialPixelValues, const TArray<FDoubleArray>& pixelWeights);

protected:
	virtual void BeginPlay() override;
		
};
