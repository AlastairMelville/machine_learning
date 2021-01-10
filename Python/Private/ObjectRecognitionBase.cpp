// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectRecognitionBase.h"
#include "Kismet/KismetDataFormatting.h"

bool UObjectRecognitionBase::TransposeMatrix(TArray<FDoubleArray>& matrix)
{
	TArray<FDoubleArray> restructuredMatrix{};
	uint16 num = matrix[0].Doubles.Num();
	restructuredMatrix.Init(FDoubleArray(), num);

	for (uint16 index = 0; index < num; ++index)
	{
		FDoubleArray matrixElem{};
		for (uint16 idx = 0; idx < matrix.Num(); ++idx)
		{
			matrixElem.Doubles.Add(matrix[idx].Doubles[index]);
		}
		restructuredMatrix[index] = matrixElem;
	}
	matrix = restructuredMatrix;
	
	
	/*FDoubleArray matrixElem{};
	for (int32 index = 0; index < matrix.Num(); ++index)
	{
		matrixElem.Doubles.Add(matrix[index].Doubles[0]);
	}
	restructuredMatrix.Add(matrixElem);
	matrix = restructuredMatrix;*/
	return true;

	return false;
}

bool UObjectRecognitionBase::MatrixMultiplication_Doubles(TArray<FDoubleArray>& OUT_matrix, const TArray<FDoubleArray>& a, const TArray<FDoubleArray>& b)
{
	for (int32 aRowIdx = 0; aRowIdx < a.Num(); ++aRowIdx)
	{
		OUT_matrix.Add(FDoubleArray());
		FDoubleArray dotProdArray = FDoubleArray();

		int32 num = a[aRowIdx].Doubles.Num();

		for (int32 bColIdx = 0; bColIdx < b.Num(); ++bColIdx)
		{
			double total = 0.0;

			// Perform a dot product calculation on this row of a and the current column of b
			for (int32 aColIdx = 0; aColIdx < num; ++aColIdx)
			{
				double aVal = a[aRowIdx].Doubles[aColIdx];
				double bVal = b[bColIdx].Doubles[aColIdx];
				double dotProductElem = aVal * bVal;

				total += dotProductElem;
			}

			double dotProduct = total / a[aRowIdx].Doubles.Num(); // Avoid divide by 0 error
			dotProdArray.Doubles.Add(dotProduct);
		}

		OUT_matrix[aRowIdx] = dotProdArray; // Could just construct the Array in an add function here
	}

	if (OUT_matrix.Num() == a.Num()) // Could also check that each row has .Num() > 0
	{
		return true;
	}

	return false;
}

bool UObjectRecognitionBase::MatrixAddition_Doubles(TArray<FDoubleArray>& OUT_matrix, const TArray<FDoubleArray>& a, const TArray<FDoubleArray>& b)
{
	if (a.Num() != b.Num()
		|| a[0].Doubles.Num() != b[0].Doubles.Num())
	{
		return false;
	}

	for (int32 rowIdx = 0; rowIdx < a.Num(); ++rowIdx)
	{
		int32 num = a[rowIdx].Doubles.Num();
		TArray<double> rowElems{};

		for (int32 colIdx = 0; colIdx < b[rowIdx].Doubles.Num(); ++colIdx)
		{
			double aElem = a[rowIdx].Doubles[colIdx];
			double bElem = b[rowIdx].Doubles[colIdx];
			rowElems.Add(aElem + bElem);
		}

		OUT_matrix.Add(FDoubleArray(rowElems));
	}

	if (OUT_matrix.Num() == a.Num()
		&& OUT_matrix[0].Doubles.Num() == a[0].Doubles.Num())
	{
		return true;
	}

	return false;
}

TArray<FDoubleArray> UObjectRecognitionBase::ReshapePixelArray(const TArray<double>& pixels, const FVector2D& newShape)
{
	TArray<FDoubleArray> pixelMatrix{};
	pixelMatrix.Init(FDoubleArray(), newShape.Y);
	uint32 currentPixelIndex = 0;
	uint32 numX = uint32(newShape.Y);
	for (uint32 rowIdx = 0; rowIdx < numX; ++rowIdx)
	{
		FDoubleArray row = FDoubleArray(newShape.X);
		uint32 numY = uint32(newShape.X);
		for (uint32 colIdx = 0; colIdx < numY; ++colIdx)
		{
			row.Doubles[colIdx] = pixels[currentPixelIndex];
			++currentPixelIndex;
		}
		pixelMatrix[rowIdx] = row;
	}
	return pixelMatrix;
}
