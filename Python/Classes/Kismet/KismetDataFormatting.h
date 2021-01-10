// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetDataFormatting.generated.h"


class GraphNode
{

};

USTRUCT(BlueprintType)
struct PYTHON_API FFloatArray
{
	GENERATED_BODY()

	FFloatArray()
		: Doubles()
	{}

	FFloatArray(TArray<float> floats)
		: Doubles(floats)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Doubles;
};

USTRUCT(BlueprintType)
struct PYTHON_API FDoubleArray
{
	GENERATED_BODY()

	FDoubleArray()
		: Doubles()
	{}

	FDoubleArray(TArray<double> doubles)
		: Doubles(doubles)
	{}

	FDoubleArray(uint32 length)
		: Doubles()
	{
		Doubles.Init(0.0, length);
	}

	UPROPERTY(EditAnywhere)
	TArray<double> Doubles;

	FDoubleArray operator - (const FDoubleArray& other) const
	{
		TArray<double> newDoubleArray{};

		int32 num = Doubles.Num();
		for (int32 index = 0; index < num; ++index)
		{
			double result = Doubles[index] - other.Doubles[index];
		}
		return FDoubleArray(newDoubleArray);
	}
};

USTRUCT(BlueprintType)
struct PYTHON_API FDoubleMatrix
{
	GENERATED_BODY()

	FDoubleMatrix()
		: DoubleArrays()
	{}

	FDoubleMatrix(TArray<FDoubleArray> doubleArrays)
		: DoubleArrays(doubleArrays)
	{}

	UPROPERTY(EditAnywhere)
	TArray<FDoubleArray> DoubleArrays;
};

USTRUCT(BlueprintType)
struct PYTHON_API FStringArray
{
	GENERATED_BODY()

	FStringArray()
		: Strings()
	{}

	FStringArray(TArray<FString> strings)
		: Strings(strings)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Strings;
};

USTRUCT(BlueprintType)
struct PYTHON_API FPredictionCallback
{
	GENERATED_BODY()

	FPredictionCallback()
		: Prediction(INDEX_NONE)
		, FinalWeights(0.0f)
		, FinalBiases(0.0f)
	{
	}

	FPredictionCallback(int32 prediction, float weights, float biases)
		: Prediction(prediction)
		, FinalWeights(weights)
		, FinalBiases(biases)
	{
	}

	int32 Prediction;

	float FinalWeights;

	float FinalBiases;
};

USTRUCT(BlueprintType)
struct PYTHON_API FWordToVecMapContainer
{
	GENERATED_BODY()

	FWordToVecMapContainer()
		: WordToVecMap()
	{
	}

	FWordToVecMapContainer(TArray<FString> keys, TArray<FDoubleArray> valueArrays)
		: WordToVecMap()
	{
		Init(keys, valueArrays);
	}

	void Init(TArray<FString> keys, TArray<FDoubleArray> valueArrays);

	const TMap<FString, FDoubleArray>& GetWordToVecMap() const { return WordToVecMap; }

	/*int32 GetWordVectorLength() const
	{
		TArray<FString> keyArray{};
		WordToVecMap.GenerateKeyArray(keyArray);
		if (keyArray.Num())
		{
			const FDoubleArray* firstKeyValue = WordToVecMap.Find(keyArray[0]);
			return firstKeyValue->Doubles.Num();
		}
		return 0;
	}*/

	bool IsValid() { return WordToVecMap.Num(); }

private:
	UPROPERTY()
	TMap<FString, FDoubleArray> WordToVecMap;
};

USTRUCT(BlueprintType)
struct PYTHON_API FTextRecognitionMath
{
	GENERATED_BODY()

FTextRecognitionMath()
	{}

	static bool Softmax(TArray<double> IN_values, TArray<double>& OUT_values);
};

UCLASS()
class PYTHON_API UKismetDataFormatting : public UObject
{
	GENERATED_BODY()

public:
	static bool GetFormattedWordData(const FString& filePath, TArray<FString>& words);

	static bool GetFormattedWordVectorData(const FString& filePath, TArray<FDoubleArray>& OUT_DoubleMatrix, TArray<FStringArray>& OUT_StringMatrix, bool bGetStringValue = false);

	static double Conv_StringToDouble(const FString& InString);

#if WITH_EDITOR
	static bool ConvertMathNotation(FString& OUT_VecElemString);
#endif // WITH_EDITOR
};
