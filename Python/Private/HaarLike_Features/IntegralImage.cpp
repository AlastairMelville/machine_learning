// Fill out your copyright notice in the Description page of Project Settings.

#include "HaarLike_Features/IntegralImage.h"


AIntegralImage::AIntegralImage()
	: IntegralImage{}
	, RowLength(0)
{
}

void AIntegralImage::CreateIntegralImage(const TArray<float>& texturePixels, const int32 textureRowLength)
{
	// 
	const int32 numPixels = texturePixels.Num();
	const int32 numRows = numPixels / textureRowLength;

	// 
	TArray<TArray<float>> pixelMatrix{};
	TArray<float> rowT{};
	rowT.Init(0.0f, textureRowLength);
	pixelMatrix.Init(rowT, numRows);
	TArray<TArray<float>> integralImage = pixelMatrix; // copy the dimensions and zeroed values from the constructed pixelMatrix

	// 
	for (int32 index = 0; index <= numPixels; ++index)
	{
		// 
		const int32 row = FMath::FloorToInt(index / textureRowLength);
		const int32 col = index % textureRowLength;
		pixelMatrix[row][col] = texturePixels[index];

		// 
		float sum = 0.0f;
		for (int32 rowIdx = 0; rowIdx <= row; ++rowIdx)
		{
			for (int32 colIdx = 0; colIdx <= col; ++colIdx)
			{
				sum += pixelMatrix[rowIdx][colIdx];
			}
		}
		integralImage[row][col] = sum;
	}

	IntegralImage = integralImage;
}

float AIntegralImage::CalculateIntegralBoxAverage(const uint8 rectangleSizeX, const uint8 rectangleSizeY, const uint8 startRow, const uint8 startCol, 
	const uint8 endRow, const uint8 endCol)
{
	if (IntegralImage.Num())
	{
		float sum = IntegralImage[endRow][endCol];

		/*const uint8 startRow = FMath::FloorToInt(startIndex / RowLength);
		const uint8 startCol = startIndex % RowLength;
		const uint8 endRow = FMath::FloorToInt(endIndex / RowLength);
		const uint8 endCol = endIndex % RowLength;*/

		// Check if there will be any above rows to subtract from the sum 
		if (startRow > 0)
		{
			sum -= IntegralImage[startRow - 1][endCol];
		}

		// Check if there will be any columns to subtract from the sum
		bool bAddBack = false;
		if (startCol > 0)
		{
			sum -= IntegralImage[endRow][startCol - 1];
			bAddBack = true;
		}
		// 
		if (bAddBack)
		{
			sum += IntegralImage[startRow - 1][startCol - 1];
		}

		return sum / (rectangleSizeX * rectangleSizeY);
	}

	return -1.0f; // -1.0f for chacking this functions success
}
