// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectDetectionPrediction.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AObjectDetectionPrediction::AObjectDetectionPrediction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AObjectDetectionPrediction::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectDetectionPrediction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AObjectDetectionPrediction::MaxPool(TArray<FDoubleArray> pixelMatrix)
{
	float maxValue = 0.0f;
	int32 indexOfMax = 0;
	//UKismetMathLibrary::MaxOfFloatArray(pixelMatrix, indexOfMax, maxValue);
	return maxValue;
}

TArray<FDoubleArray> AObjectDetectionPrediction::CalculateConvolutionalLayer(const TArray<FDoubleArray>& imagePixelValues, const TArray<FDoubleArray>& imagePixelWeights, 
	const FVector2D& dims)
{
	/*const int32 imageHeight = imagePixelValues.Num();
	const int32 imageWidth = imagePixelValues[0].Doubles.Num();
	const int32 numX = imageHeight - (dims.X - 1);
	const int32 numY = imageWidth - (dims.Y - 1);

	TArray<FDoubleArray> newPixelValues{};
	TArray<double> std = TArray<double>{0.0, numY};
	newPixelValues.Init(std, numX);

	for (int32 idxX = 0; idxX < numX; ++idxX)
	{
		TArray<double> tempPixelArray{std};
		for (int32 idxY = 0; idxY < numY; ++idxY)
		{
			TArray<FDoubleArray> tempPixelValueArray{};
			TArray<FDoubleArray> tempPixelWeightArray{};
			GetPixelValuesAndWeights(imagePixelValues, imagePixelWeights, FVector2D(idxX, idxY), dims, tempPixelValueArray, tempPixelWeightArray);

			double value = CalculateConvolutionalLayerElement(tempPixelValueArray, tempPixelWeightArray);
			tempPixelArray[idxY] = value;
		}
		newPixelValues[idxX] = tempPixelArray;
	}

	return newPixelValues;*/

	return TArray<FDoubleArray>();
}

TArray<FDoubleArray> AObjectDetectionPrediction::GetPixelValuesAndWeights(const TArray<FDoubleArray>& imagePixelValues, const TArray<FDoubleArray>& imagePixelWeights, 
	const FVector2D& startingPixelLoc, const FVector2D& dims, TArray<FDoubleArray>& OUT_pixelValues, TArray<FDoubleArray>& OUT_pixelWeights)
{
	/*const int32 imageSectionHeight = imagePixelValues.Num();
	const int32 imageSectionWidth = imagePixelValues[0].Doubles.Num();
	const int32 numX = imageSectionHeight - (dims.X - 1);
	const int32 numY = imageSectionWidth - (dims.Y - 1);

	TArray<double> std = TArray<double>{ 0.0, numY };
	TArray<FDoubleArray> tempPixelMatrix{std, numX};
	TArray<FDoubleArray> tempPixelWeightsMatrix{ std, numX };
	
	for (int32 idxX = 0; idxX < dim.X; ++idxX)
	{
		TArray<double> tempPixelArray{0.0, dim.X};
		TArray<double> tempPixelWeightsArray{0.0, dim.X};
		for (int32 idxY = 0; idxY < dim.Y; ++idxY)
		{
			tempPixelArray[idxY] = imagePixelValues[startingPixelLoc + idxY];
			tempPixelWeightsArray[idxY] = imagePixelWeights[startingPixelLoc + idxY];
		}
		tempPixelMatrix[indX] = tempPixelArray;
		tempPixelWeightsMatrix[indX] = tempPixelWeightsArray;
	}
	

	return tempPixelMatrix;*/

	return TArray<FDoubleArray>();
}

double AObjectDetectionPrediction::CalculateConvolutionalLayerElement(const TArray<FDoubleArray>& originialPixelValues, const TArray<FDoubleArray>& pixelWeights)
{


	return 0.0;
}
