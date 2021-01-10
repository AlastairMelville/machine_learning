// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectRecognitionBase.h"
#include "TextRecognition.generated.h"


class UWordToVecFormatting;
struct FStringArray;
struct FFloatArray;

UCLASS()
class UTextRecognitionBase : public UObjectRecognitionBase
{
	GENERATED_BODY()

public:
	UTextRecognitionBase();

	static bool SetWordToVecMapContainer(FWordToVecMapContainer& OUT_wordToVecMapContainer, const FString wordsFilename = "FormattedWords_ForWordToVec.txt", 
		const FString wordVectorFilename = "FormattedWordToVec.txt");

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWordToVecMapContainer WordToVecMapContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WordsFilename;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WordVectorFilename;
};

UCLASS()
class UWordSimilatity : public UTextRecognitionBase
{
	GENERATED_BODY()

public:
	UWordSimilatity();

	float CompareWords(const FString& first, const FString& second);

	/* Creates an "A is to B, as C is to__" analogy. */
	FString FollowAnalogyForOtherWord(const FString& pairOneA, const FString& pairOneB, const FString& pairTwoA);

	template<typename dataType>
	static float CalculateCosineSimilarity(const dataType& u, const dataType& v)
	{
		float distance = 0.0f;

		// 
		TArray<dataType> dotProdArray{};

		TArray<dataType> uMatrix{};
		uMatrix.Add(dataType(u));

		TArray<dataType> vMatrix{};
		vMatrix.Add(dataType(v));

		MatrixMultiplication<dataType>(dotProdArray, uMatrix, vMatrix);
		float dotProd = dotProdArray[0].Doubles[0]; // The matrix should only consist on 1 element.

													// 
		float uTotal = 0.0f;
		int32 uNum = u.Doubles.Num();
		for (const float& elem : u.Doubles)
		{
			float uSqr = elem * elem;
			uTotal += uSqr;
		}
		float uNormalised = FMath::Sqrt(uTotal);

		float vTotal = 0.0f;
		int32 vNum = v.Doubles.Num();
		for (const float& elem : v.Doubles)
		{
			float vSqr = elem * elem;
			vTotal += vSqr;
		}
		float vNormalised = FMath::Sqrt(vTotal);

		// 
		float cosineSimilarity = dotProd / (uNormalised * vNormalised);

		return cosineSimilarity;
	}
};

UCLASS()
class PYTHON_API USentenceRecognition : public UTextRecognitionBase
{
	GENERATED_BODY()

public:
	USentenceRecognition();

	static FPredictionCallback Predict(const FString& sentence, TArray<FDoubleArray> weights, TArray<double> biases,
		const TArray<int32> labels = TArray<int32>());	

protected:
#if WITH_EDITOR
	UPROPERTY(EditAnywhere, Category = "Testing")
	UWordToVecFormatting* WordToVecFormatting_DA;

	UPROPERTY(EditAnywhere, Category = "Testing")
	uint32 bTestGetFormattedData : 1;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
