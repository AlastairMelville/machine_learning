// Fill out your copyright notice in the Description page of Project Settings.

#include "TextRecognition.h"
#include "WordSimilarity.h"
//#include "Kismet/KismetDataFormatting.h"

//extern TAutoConsoleVariable<int32> CVarObjectREcognitionCalculationsPerSecond;

UTextRecognitionBase::UTextRecognitionBase()
	: WordToVecMapContainer(FWordToVecMapContainer())
	, WordsFilename("FormattedWords_ForWordToVec.txt")
	, WordVectorFilename("FormattedWordToVec.txt.csv")
{
}

bool UTextRecognitionBase::SetWordToVecMapContainer(FWordToVecMapContainer& OUT_wordToVecMapContainer, const FString wordsFilename, const FString wordVectorFilename)
{
	TArray<FString> words{};
	if (UKismetDataFormatting::GetFormattedWordData(FPaths::ProjectSavedDir() + wordsFilename, words))
	{
		TArray<FStringArray> wordVectorsAsStrings{}; // Not filled
		TArray<FDoubleArray> wordVectors{};
		if (UKismetDataFormatting::GetFormattedWordVectorData(FPaths::ProjectSavedDir() + wordVectorFilename, wordVectors, wordVectorsAsStrings))
		{
			OUT_wordToVecMapContainer = FWordToVecMapContainer(words, wordVectors);

			if (OUT_wordToVecMapContainer.IsValid())
			{
				return true;
			}
		}
	}

	return false;
}

UWordSimilatity::UWordSimilatity()
{
}

float UWordSimilatity::CompareWords(const FString& first, const FString& second)
{
	FWordToVecMapContainer wordToVecMapContainer;
	if (SetWordToVecMapContainer(wordToVecMapContainer)) // TODO: Temp placement
	{
		// 
		const TMap<FString, FDoubleArray>& localWordToVecMap = wordToVecMapContainer.GetWordToVecMap();

		// 
		const FDoubleArray* firstWordVectors = localWordToVecMap.Find(first);
		const FDoubleArray* secondWordVectors = localWordToVecMap.Find(second);

		return CalculateCosineSimilarity<FDoubleArray>(*firstWordVectors, *secondWordVectors);
	}

	return 0.0f;
}

FString UWordSimilatity::FollowAnalogyForOtherWord(const FString& pairOneA, const FString& pairOneB, const FString& pairTwoA)
{
	const FString pairOneALower = pairOneA.ToLower();
	const FString pairOneBLower = pairOneB.ToLower();
	const FString pairTwoALower = pairTwoA.ToLower();

	FString bestWord = "";

	FWordToVecMapContainer wordToVecMapContainer;
	if (SetWordToVecMapContainer(wordToVecMapContainer)) // TODO: Temp placement
	{
		// 
		const TMap <FString, FDoubleArray>& localWordToVecMap = wordToVecMapContainer.GetWordToVecMap();

		// 
		const FDoubleArray* pairOneAEmbedding = localWordToVecMap.Find(pairOneALower);
		const FDoubleArray* pairOneBEmbedding = localWordToVecMap.Find(pairOneBLower);
		const FDoubleArray* pairTwoAEmbedding = localWordToVecMap.Find(pairTwoALower);

		// 
		float maxCosineSimularity = -100.0f;

		// 
		TArray<FString> wordToVecKeys;
		localWordToVecMap.GenerateKeyArray(wordToVecKeys);
		for (int32 index = 0; index < wordToVecKeys.Num(); ++index)
		{
			// To avoid best_word being one of the input words, pass on them.
			if (wordToVecKeys[index] == pairOneA || wordToVecKeys[index] == pairOneB || wordToVecKeys[index] == pairTwoA)
			{
				continue;
			}

			// Compute cosine similarity between the vector (e_b - e_a) and the vector ((w's vector representation) - e_c)
			const FDoubleArray bSubA = *pairOneBEmbedding - *pairOneAEmbedding;

			const FDoubleArray chosenWordVector = localWordToVecMap[wordToVecKeys[index]];
			const FDoubleArray wordSub = chosenWordVector - *pairTwoAEmbedding;

			float cosineSimilarity = CalculateCosineSimilarity<FDoubleArray>(bSubA, wordSub);

			if (cosineSimilarity > maxCosineSimularity)
			{
				maxCosineSimularity = cosineSimilarity;
				bestWord = wordToVecKeys[index];
			}
		}

		return bestWord;
	}

	return bestWord;
}

USentenceRecognition::USentenceRecognition()
{
}

FPredictionCallback USentenceRecognition::Predict(const FString& sentence, TArray<FDoubleArray> weights, TArray<double> biases,
	const TArray<int32> labels /* = TArray<int32>() */)
{
	//int32 numData = data.Num();
	FPredictionCallback predictionCallback = FPredictionCallback();
	
	// TEMP Start
	TArray<FDoubleArray> modifiedWeights{};
	modifiedWeights.SetNum(weights.Num());
	for (int32 matIdx = 0; matIdx < modifiedWeights.Num(); ++matIdx)
	{
		for (int32 idx = 0; idx < 15; ++idx)
		{
			modifiedWeights[matIdx].Doubles.Add(weights[matIdx].Doubles[idx]);
		}
	} 
	// TEMP End

	FWordToVecMapContainer wordToVecMapContainer;
	if (SetWordToVecMapContainer(wordToVecMapContainer)) // TODO: Temp placement
	{
		const TMap<FString, FDoubleArray>& localWordToVecMap = wordToVecMapContainer.GetWordToVecMap();
		//int32 wordVectorLength = wordToVecMap.GetWordVectorLength();
		
		// Extract and extract each sentence passed into this function (may just be one)
		//for (int32 dataIdx = 0; dataIdx < numData; ++dataIdx)
		{
			FString lowerCaseDataElem = sentence.ToLower();
			TArray<FString> sentenceWords = UKismetStringLibrary::ParseIntoArray(lowerCaseDataElem, TEXT(" "));

			TArray<double> totals;
			TArray<double> averages;
			totals.Init(0.0, 15);
			averages.Init(0.0, 15);
			int32 numWords = sentenceWords.Num();
			int32 wordVectorLength = 0;

			// Calculate the totals for each vector dimension
			for (int32 wordIdx = 0; wordIdx < numWords; ++wordIdx)
			{
				const FDoubleArray* sentenceVectors = localWordToVecMap.Find(sentenceWords[wordIdx]); // Get word vectors for each word in the curremt sentence
				if (sentenceVectors)
				{
					wordVectorLength = sentenceVectors->Doubles.Num();

					for (int32 vectorIdx = 0; vectorIdx < wordVectorLength; ++vectorIdx)
					{
						totals[vectorIdx] += sentenceVectors->Doubles[vectorIdx];
					}
				}
				else
				{
					UE_LOG(LogScript, Warning, TEXT("Word (%s) not found"), *sentenceWords[wordIdx]);
				}
			}

			// Calculate the averages for each vector dimension
			for (int32 vectorIdx = 0; vectorIdx < wordVectorLength; ++vectorIdx)
			{
				averages[vectorIdx] = totals[vectorIdx] / numWords;
			}
			TArray<FDoubleArray> averagesMatrix{};
			averagesMatrix.Add(FDoubleArray(averages));

			TArray<FDoubleArray> z{};
			MatrixMultiplication_Doubles(z, modifiedWeights, averagesMatrix);
			TransposeMatrix(z);


			TArray<FDoubleArray> z2{};
			TArray<FDoubleArray> biasesMatrx{};
			biasesMatrx.Add(FDoubleArray(biases));
			MatrixAddition_Doubles(z2, z, biasesMatrx);

			TArray<double> softmaxUnitVector{};
			int32 highestIndex = INDEX_NONE;
			if (FTextRecognitionMath::Softmax(z2[0].Doubles, softmaxUnitVector))
			{
				float maxValue = FMath::Max(softmaxUnitVector, &highestIndex);
			}
			else
			{

			}

			double TEMP_minValueForDecisionToBeBasedOnThis = 0.4f; // TODO: Make this a suitable member variable
			if (highestIndex > INDEX_NONE && softmaxUnitVector[highestIndex] > TEMP_minValueForDecisionToBeBasedOnThis)
			{
				// TODO: Select a 
			}
		}
	}

	return predictionCallback;
}

#if WITH_EDITOR
void USentenceRecognition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(USentenceRecognition, bTestGetFormattedData))
		{
			if (bTestGetFormattedData)
			{
				if (WordToVecFormatting_DA)
				{					
					WordToVecFormatting_DA->TestEditorOnly_GetFormattedData("SentenceSeqModel_XData.txt", EFormattingType::Sentences);
					if (!WordToVecFormatting_DA->WordData.Num())
					{
						return;
					}
					const FString wordData = WordToVecFormatting_DA->WordData[0];
					WordToVecFormatting_DA->WordData = TArray<FString>();

					WordToVecFormatting_DA->TestEditorOnly_GetFormattedData("SentenceSeqModelWeights.txt", EFormattingType::Numbers);
					const TArray<FDoubleArray> weights = WordToVecFormatting_DA->VectorValues;
					WordToVecFormatting_DA->VectorValues = TArray<FDoubleArray>();

					WordToVecFormatting_DA->TestEditorOnly_GetFormattedData("SentenceSeqModelBiases.txt", EFormattingType::Numbers);
					const TArray<double> baises = WordToVecFormatting_DA->VectorValues[0].Doubles;
					WordToVecFormatting_DA->VectorValues = TArray<FDoubleArray>();

					Predict(wordData, weights, baises);
				}

				bTestGetFormattedData = false;
			}
		}
	}
}
#endif // WITH_EDITOR
