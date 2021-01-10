// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/Interface.h"
#include "ObjectRecognitionBase.generated.h"


struct FDoubleArray;
struct FFloatArray;

UCLASS()
class PYTHON_API UObjectRecognitionBase : public UDataAsset
{
	GENERATED_BODY()
	
public:
	static bool TransposeMatrix(TArray<FDoubleArray>& matrix);

	static bool MatrixMultiplication_Doubles(TArray<FDoubleArray>& OUT_matrix, const TArray<FDoubleArray>& a, const TArray<FDoubleArray>& b);

	static bool MatrixAddition_Doubles(TArray<FDoubleArray>& OUT_matrix, const TArray<FDoubleArray>& a, const TArray<FDoubleArray>& b);

	static TArray<FDoubleArray> ReshapePixelArray(const TArray<double>& pixels, const FVector2D& newShape);

	template<typename dataType>
	static bool MatrixMultiplication(TArray<dataType>& OUT_matrix, const TArray<dataType>& a, const TArray<dataType>& b)
	{
		for (int32 aRowIdx = 0; aRowIdx < a.Num(); ++aRowIdx)
		{
			OUT_matrix.Add(dataType());
			dataType dotProdArray = dataType();

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
};
