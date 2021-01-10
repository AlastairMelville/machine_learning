// Fill out your copyright notice in the Description page of Project Settings.

#include "WordSimilarity.h"
#include "Paths.h"
#include "FileHelper.h"
#include "Kismet/KismetDataFormatting.h"
#include "Kismet/KismetStringLibrary.h"

#if WITH_EDITOR
void UWordToVecFormatting::FormatText(const FString& fromFilename, const FString& toFilename)
{
	FString rawTextFilePath = FPaths::ProjectSavedDir() + fromFilename;
	FString formattedTextFilePath = FPaths::ProjectSavedDir() + toFilename;

	if (FPaths::FileExists(rawTextFilePath) && FPaths::FileExists(formattedTextFilePath))
	{
		FString rawText;
		FFileHelper::LoadFileToString(rawText, *rawTextFilePath);
		if (rawText.Len())
		{
			rawText = rawText.ToLower(); // Remove upper case letters

										 // Remove unwanted triple and double spaces. Note: remove the larger spaces first - removing a double space could which is within a truple could just create another double space that gets missed.
			rawText = rawText.Replace(TEXT("   "), TEXT(" "));
			rawText = rawText.Replace(TEXT("  "), TEXT(" "));

			// Remove unwanted punctuation
			rawText = rawText.Replace(TEXT("\r"), TEXT(""));
			rawText = rawText.Replace(TEXT("\n"), TEXT(""));
			rawText = rawText.Replace(TEXT("]"), TEXT(""));
			rawText = rawText.Replace(TEXT(";"), TEXT(""));
			rawText = rawText.Replace(TEXT(":"), TEXT(""));
			rawText = rawText.Replace(TEXT("!"), TEXT(""));
			rawText = rawText.Replace(TEXT("?"), TEXT(""));
			rawText = rawText.Replace(TEXT(")"), TEXT(""));
			rawText = rawText.Replace(TEXT("("), TEXT(""));
			rawText = rawText.Replace(TEXT("'"), TEXT(""));

			switch (EFormattingType::Numbers) //(FormattingType)
			{
			case EFormattingType::Sentences:
			{
				rawText = rawText.Replace(TEXT("["), TEXT(""));
				rawText = rawText.Replace(TEXT(","), TEXT(""));
				rawText = rawText.Replace(TEXT("."), TEXT(""));
				break;
			}
			case EFormattingType::WordList:
			{
				rawText = rawText.Replace(TEXT("["), TEXT(""));
				rawText = rawText.Replace(TEXT(" "), TEXT(","));
				rawText = rawText.Replace(TEXT("."), TEXT(""));
				break;
			}
			case EFormattingType::Numbers:
			{
				rawText = rawText.Replace(TEXT("[ "), TEXT("\n"));
				rawText = rawText.Replace(TEXT("["), TEXT("\n"));
				rawText = rawText.Replace(TEXT(" "), TEXT(","));
				break;
			}
			default:
			{
				break;
			}
			}

			/* TODO: Could count certain puncuation (e.g. question marks) to track what kind of lanugague is commonly used - e.g. increase AI's rate of asking
			questions in this certain contexts. */

			FFileHelper::SaveStringToFile(rawText, *formattedTextFilePath);
		}
	}
	else
	{

	}
}

void UWordToVecFormatting::FormatWordToVecData(const FString& fromFilename, const FString& toFilename)
{
	FString rawWordToVecFilePath = FPaths::ProjectSavedDir() + fromFilename;
	FString formattedWordToVecFilePath = FPaths::ProjectSavedDir() + toFilename;

	if (FPaths::FileExists(rawWordToVecFilePath) && FPaths::FileExists(formattedWordToVecFilePath))
	{
		TArray<FString> rawData;
		FFileHelper::LoadFileToStringArray(rawData, *rawWordToVecFilePath);
		if (rawData.Num())
		{
			int32 length = rawData.Num();
			for (int32 vecIdx = 0; vecIdx < length; ++vecIdx)
			{
				TArray<FString> vectorElements = UKismetStringLibrary::ParseIntoArray(rawData[vecIdx], TEXT(","));

				int32 elemLen = vectorElements.Num();
				for (int32 index = 0; index < elemLen; ++index)
				{
					if (vectorElements[index].Len())
					{
						FString vectorElem = vectorElements[index];
						UKismetDataFormatting::ConvertMathNotation(vectorElem);
						vectorElements[index] = vectorElem + ","; // Adding "," converts it back into a .csv format
					}
				}
				// Convert string array back to single string
				rawData[vecIdx].Empty(); // Get rid of old content, otherwise you will just add to it.
				for (const FString& vectorStr : vectorElements)
				{
					rawData[vecIdx].Append(vectorStr);
				}

				FFileHelper::SaveStringArrayToFile(rawData, *formattedWordToVecFilePath);
			}
		}
	}
	else
	{

	}
}

bool UWordToVecFormatting::GetFormattedData(const FString& fromFilename, TArray<FDoubleArray>& OUT_vectorValues, TArray<FString>& OUT_wordData, 
	EFormattingType overrideFormattingType /* = EFormattingType::Default */)
{
	FString filePath = FPaths::ProjectSavedDir() + fromFilename;

	EFormattingType formattingType = EFormattingType::Default;
	if (overrideFormattingType != EFormattingType::Default)
	{
		formattingType = overrideFormattingType;
	}

	switch (formattingType)
	{
	case EFormattingType::Numbers:
	{
		TArray<FStringArray> tempVecAsStrings{};
		return UKismetDataFormatting::GetFormattedWordVectorData(filePath, OUT_vectorValues, tempVecAsStrings, true);
		break;
	}
	case EFormattingType::WordList:
	{
		return UKismetDataFormatting::GetFormattedWordData(filePath, OUT_wordData);
		break;
	}
	case EFormattingType::Sentences:
	{
		return UKismetDataFormatting::GetFormattedWordData(filePath, OUT_wordData);
		break;
	}
	default:
	{
		break;
	}
	}

	return false;
}

bool UWordToVecFormatting::TestEditorOnly_GetFormattedData(const FString& fromFilename, EFormattingType overrideFormattingType /* = EFormattingType::Default */)
{
	FString filePath = FPaths::ProjectSavedDir() + fromFilename;

	EFormattingType formattingType = FormattingType;
	if (overrideFormattingType != EFormattingType::Default)
	{
		formattingType = overrideFormattingType;
	}

	switch (formattingType)
	{
	case EFormattingType::Numbers:
	{
		return UKismetDataFormatting::GetFormattedWordVectorData(filePath, VectorValues, VectorValuesAsStrings, true);
		break;
	}
	case EFormattingType::WordList:
	{
		return UKismetDataFormatting::GetFormattedWordData(filePath, WordData);
		break;
	}
	case EFormattingType::Sentences:
	{
		return UKismetDataFormatting::GetFormattedWordData(filePath, WordData);
		break;
	}
	default:
	{
		break;
	}
	}

	return false;
}

void UWordToVecFormatting::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWordToVecFormatting, bFormatText))
		{
			if (bFormatText)
			{
				FormatText(FromFileName, ToFileName);

				bFormatText = false;
			}
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWordToVecFormatting, bFormatWordToVecData))
		{
			if (bFormatWordToVecData)
			{
				FormatWordToVecData(FromFileName, ToFileName);

				bFormatWordToVecData = false;
			}
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWordToVecFormatting, bTestGetFormattedData))
		{
			if (bTestGetFormattedData)
			{
				TestEditorOnly_GetFormattedData(FromFileName);

				bTestGetFormattedData = false;
			}
		}
	}
}

void UCustomTextFormatting::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCustomTextFormatting, bFormat))
		{
			if (bFormat)
			{
				FString locationFilepath = FPaths::ProjectSavedDir() + LocationFilename;
				FString destinationFilepath = FPaths::ProjectSavedDir() + DestinationFilename;
				if (FPaths::FileExists(locationFilepath))
				{
					FString fileString;
					if (FFileHelper::LoadFileToString(fileString, *locationFilepath))
					{
						for (int32 index = 0; index < TextSwitches.Num(); ++index)
						{
							if (TextSwitches[index].Remove == "\\n")
							{
								fileString = fileString.Replace(TEXT("\n"), *TextSwitches[index].Replacement);
								continue;
							}
							if (TextSwitches[index].Remove == "\\r")
							{
								fileString = fileString.Replace(TEXT("\r"), *TextSwitches[index].Replacement);
								continue;
							}
							if (TextSwitches[index].Remove == "\\r\n")
							{
								fileString = fileString.Replace(TEXT("\r"), TEXT(""));
								fileString = fileString.Replace(TEXT("\n"), *TextSwitches[index].Replacement);
								continue;
							}
							if (TextSwitches[index].Remove == "\\n\r")
							{
								fileString = fileString.Replace(TEXT("\n"), TEXT(""));
								fileString = fileString.Replace(TEXT("\r"), *TextSwitches[index].Replacement);
								continue;
							}

							if (int32 idx = TextSwitches[index].Replacement.Find("\\n") > INDEX_NONE)
							{
								TextSwitches[index].Replacement.RemoveAt(idx);

								fileString = fileString.Replace(*TextSwitches[index].Remove, TEXT("\n"));
								continue;
							}
							if (int32 idx = TextSwitches[index].Replacement.Find("\\r") > INDEX_NONE)
							{
								TextSwitches[index].Replacement.RemoveAt(idx);

								fileString = fileString.Replace(*TextSwitches[index].Remove, TEXT("\r"));
								continue;
							}

							fileString = fileString.Replace(*TextSwitches[index].Remove, *TextSwitches[index].Replacement);
						}

						FFileHelper::SaveStringToFile(fileString, *destinationFilepath);//, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), );
					}
				}

				bFormat = false;
			}
		}
	}
}
#endif // WITH_EDITOR
