// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetDataFormatting.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Kismet/KismetStringLibrary.h"


void FWordToVecMapContainer::Init(TArray<FString> keys, TArray<FDoubleArray> valueArrays)
{
	for (int32 index = 0; index < keys.Num(); ++index)
	{
		FDoubleArray doubleMatrix = valueArrays[index];
		WordToVecMap.Add(keys[index], doubleMatrix);
	}
}

bool FTextRecognitionMath::Softmax(TArray<double> IN_values, TArray<double>& OUT_values)
{
	/* Compute softmax values for each sets of scores in values. */
	TArray<double> outValues{};
	int32 inValuesNum = IN_values.Num();
	outValues.Init(0.0, inValuesNum);

	TArray<double> exponentials{};
	exponentials.Init(0.0, inValuesNum);
	double expsSum = 0.0;

	// Get max of in values
	int32 maxValIndex = -1;
	float maxValue = 0.0f;
	//UKismetMathLibrary::MaxOfFloatArray(IN_values, maxValIndex, maxValue);
	maxValue = FMath::Max(IN_values, &maxValIndex);

	for (int32 index = 0; index < inValuesNum; ++index)
	{
		exponentials[index] = UKismetMathLibrary::Exp(float(IN_values[index]) - maxValue);
		expsSum += exponentials[index];
	}

	for (int32 index = 0; index < inValuesNum; ++index)
	{
		outValues[index] = exponentials[index] / expsSum;
	}

	OUT_values = outValues;
	return true;

	/*e_x = np.exp(x - np.max(x));
	return e_x / e_x.sum();*/


	return false;
}

bool UKismetDataFormatting::GetFormattedWordData(const FString& filePath, TArray<FString>& words)
{
	if (FPaths::FileExists(filePath))
	{
		FString tempString{};

		FFileHelper::LoadFileToString(tempString, *filePath);

		if (tempString.Len())
		{
			TArray<FString> tempComaSepStringArr = UKismetStringLibrary::ParseIntoArray(tempString, TEXT(","));
			if (tempComaSepStringArr.Num())
			{
				for (const FString& word : tempComaSepStringArr)
				{
					words.Add(word);
				}
				return true;
			}
		}
	}

	return false;
}

bool UKismetDataFormatting::GetFormattedWordVectorData(const FString& filePath, TArray<FDoubleArray>& OUT_DoubleMatrix, TArray<FStringArray>& OUT_StringMatrix,
	bool bGetStringValue /* = false */)
{
	if (FPaths::FileExists(filePath))
	{
		TArray<FString> tempStringArray{};

		FFileHelper::LoadFileToStringArray(tempStringArray, *filePath);

		if (tempStringArray.Num())
		{
			for (int32 index = 0; index < tempStringArray.Num(); ++index)
			{
				TArray<FString> tempComaSepStringArr = UKismetStringLibrary::ParseIntoArray(tempStringArray[index], TEXT(","));
				if (tempComaSepStringArr.Num())
				{
					{
						TArray<double> vectorValues{};
						for (const FString& string : tempComaSepStringArr)
						{
							vectorValues.Add(Conv_StringToDouble(string));
						}

						FDoubleArray tempDoubleMatrixElem = FDoubleArray(vectorValues);
						OUT_DoubleMatrix.Add(tempDoubleMatrixElem);
					}

					if (bGetStringValue)
					{
						FStringArray tempStringMatrixElem = FStringArray(tempComaSepStringArr);
						OUT_StringMatrix.Add(tempStringMatrixElem);
					}
				}
			}

			return true;
		}
	}

	return false;
}

double UKismetDataFormatting::Conv_StringToDouble(const FString& InString)
{
	return FCString::Atod(*InString);
}

#if WITH_EDITOR
bool UKismetDataFormatting::ConvertMathNotation(FString& OUT_ElemString)
{
	TArray<FString> parsed{};
	OUT_ElemString.ParseIntoArray(parsed, TEXT("e"));
	if (parsed.Num())
	{
		FString valueString = parsed[0];
		FString valueModifierString = parsed.Last();
		bool bNegativeNotation = valueModifierString.Contains("-");
		valueModifierString.RemoveAt(0);

		int32 value = UKismetStringLibrary::Conv_StringToInt(valueModifierString);

		// Start the process of moving the decimal point based on the above result
		int32 charIndex = -1;
		TCHAR* chaa = TEXT(".");
		valueString.FindChar(*chaa, charIndex);
		if (charIndex > INDEX_NONE)
		{
			valueString.RemoveAt(charIndex);
			if (bNegativeNotation)
			{
				int32 extra = valueString.Contains("-") ? 1 : 0;
				int32 significantNumberCount = charIndex - extra;

				for (int32 i = 0; i < (value - significantNumberCount); ++i)
				{
					valueString.InsertAt(extra, "0");
				}
				valueString.InsertAt(extra, "0.");
			}
			else
			{
				valueString.InsertAt(charIndex + value, ".");
			}

			OUT_ElemString = valueString;
			return true;
		}
	}

	return false;
}
#endif // WITH_EDITOR
