// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonCommunictionsActor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "VictoryBPFunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"

#include "TextImageSearch.h"
#include "WordSimilarity.h"
#include "Kismet/KismetDataFormatting.h"
#include "TextRecognition.h"

TArray<FString> APythonCommunictionsActor::FilesWaitingFor{};
int32 APythonCommunictionsActor::FrameNumber = 0;

APythonCommunictionsActor::APythonCommunictionsActor()
	: RenderTexture(nullptr)
	, IdentifiedPersons{}
	, ImageDimensions(0.0f, 0.0f)
	, FullFilepath("C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/thefile1.txt")
	, FilepathExtension(".txt")
	, Pixels{}
	, CurrentPixelNum(0)
{
	PrimaryActorTick.bCanEverTick = true;

}

void APythonCommunictionsActor::BeginPlay()
{
	Super::BeginPlay();

}

void APythonCommunictionsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we are waiting for files to be processed
	if (FilesWaitingFor.Num())
	{
		// Iterate through all files in the relevant directory, trying to find filenames that conatin FilesWaitingFor elements
		TArray<FString> OUT_DetectionFiles{};
		if (UVictoryBPFunctionLibrary::JoyFileIO_GetFiles(OUT_DetectionFiles, "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/Outbox/", "*"))
		{
			for (const FString& filename : OUT_DetectionFiles)
			{
				const int32 numFiles = FilesWaitingFor.Num();
				for (int32 index = 0; index < numFiles; ++index)
				{
					if (filename.Contains(FilesWaitingFor[index]))
					{
						// If we were waiting for this file
						const FString filepath = "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/Outbox/" + filename;
						if (RetrieveObjLocsForFrameImage(filepath))
						{
							if (FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*filepath))
							{
								UE_LOG(LogScript, Warning, TEXT("As planned, we deleted %s"), *filepath);
							}
						}
					}
				}
			}
		}
	}

	TArray<FString> OUT_IdentificationFiles{};
	if (UVictoryBPFunctionLibrary::JoyFileIO_GetFiles(OUT_IdentificationFiles, "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/FaceRecognition/Outbox/", "*"))
	{
		for (const FString& filename : OUT_IdentificationFiles)
		{
			const FString filepath = "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/FaceRecognition/Outbox/" + filename;

			TArray<FString> fileContents;
			if (FFileHelper::LoadFileToStringArray(fileContents, *filepath) && fileContents.Num() > 1)
			{
				IdentifiedPersons.Add(FIdentificationInfo(fileContents[0], UKismetStringLibrary::Conv_StringToFloat(fileContents[1])));

				// If we were waiting for this file
				if (FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*filepath))
				{
					UE_LOG(LogScript, Warning, TEXT("As planned, we deleted %s"), *filepath);
				}
			}
		}
	}
}

bool APythonCommunictionsActor::ExportFrameImage(const UTextureRenderTarget2D* renderTexture, const FVector2D imageDimensions, const FString suspectedPerson,
	const FString filepath)
{
	if (renderTexture)
	{
		check(IsInGameThread());
		FTextureRenderTargetResource* res = static_cast<FTextureRenderTargetResource*>(renderTexture->Resource);
		if (res)
		{
			TArray<FColor> pixels{};
			res->ReadPixels(pixels);

			/*FString exportStringArray;
			for (int32 index = 0; index < pixels.Num(); ++index)
			{
				exportStringArray += (UKismetStringLibrary::Conv_IntToString(pixels[index].R) + ",");
				exportStringArray += (UKismetStringLibrary::Conv_IntToString(pixels[index].G) + ",");
				exportStringArray += (UKismetStringLibrary::Conv_IntToString(pixels[index].B) + ",");
			}*/
			
			FString errorString;
			FString frameNumStr = UKismetStringLibrary::Conv_IntToString(FrameNumber++);
			int8 extraDigits = FMath::Clamp(3 - frameNumStr.Len(), 0, 3);
			for (int8 index = 0; index < extraDigits; ++index)
			{
				frameNumStr = "0" + frameNumStr;
			}

			FString filename = frameNumStr + suspectedPerson;
			FilesWaitingFor.Add(filename);
						
			UVictoryBPFunctionLibrary::Victory_SavePixels_FColor(FPaths::ProjectSavedDir() + "ImageRecognition/" + frameNumStr + "Image", imageDimensions.X, imageDimensions.Y, pixels, false, true, errorString); // Temp Keep a local copy for subsequent analysis
			return UVictoryBPFunctionLibrary::Victory_SavePixels_FColor((filepath + filename), imageDimensions.X, imageDimensions.Y, pixels, false, true, errorString);

			/*FString filepathTwo = "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/testpixelfromfile.csv";
			if (FPaths::FileExists(filepathTwo))
			{
				FFileHelper::SaveStringToFile(exportStringArray, *filepathTwo);
			}*/
		}
	}

	return false;
}

bool APythonCommunictionsActor::RetrieveObjLocsForFrameImage(const FString& filepath)
{
	TArray<FString> fileContents;
	if (FFileHelper::LoadFileToStringArray(fileContents, *filepath))
	{
		if (fileContents[0] == "person")
		{
			bool bIsValid = false;
			int32 width = 0;
			int32 height = 0;
			TArray<FColor> outPixels{};

			if (UVictoryBPFunctionLibrary::Victory_LoadTexture2D_FromFile_Pixels(FPaths::ProjectSavedDir() + "ImageRecognition/" + "000Image.png" /*ImageFilepath*/, EJoyImageFormats::PNG, bIsValid, width, height, outPixels))
			{
				bool bBothVecsValid = false;
				FVector2D startLoc;
				FVector2D endLoc;
				bBothVecsValid = MyStringToVector2DConv(fileContents[2], startLoc);
				bBothVecsValid = MyStringToVector2DConv(fileContents[3], endLoc);

				if (bBothVecsValid)
				{
					int32 relevantHeight = endLoc.Y - startLoc.Y;
					int32 relevantWidth = endLoc.X - startLoc.X;
					int32 numRelevantPixels = relevantHeight * relevantWidth;
					TArray<FColor> relevantPixels;
					relevantPixels.Init(FColor(), numRelevantPixels);

					int32 index = 0;
					for (int32 idxH = 0; idxH < relevantHeight; ++idxH)
					{
						for (int32 idxW = 0; idxW < relevantWidth; ++idxW)
						{
							relevantPixels[index] = outPixels[(idxH * width) + (startLoc.X + idxW)];

							++index;
						}
					}
										
					UTextImageSearch::ResizePixelArray(relevantPixels, relevantWidth, relevantHeight, 96, 96);
					relevantWidth = relevantHeight = 96; // TODO

					TArray<FString> parsedFilepath{};
					filepath.ParseIntoArray(parsedFilepath, TEXT("/"));
					const FString filename = parsedFilepath.Last();

					const FString outFilepath = "C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/FaceRecognition/Inbox/" + filename;
					FString errorString;
					return UVictoryBPFunctionLibrary::Victory_SavePixels_FColor(outFilepath, relevantWidth, relevantHeight, relevantPixels, false, true, errorString);
				}
							

				/*UObjectRecognitionBase::ReshapePixelArray();

				TArray<float> UImageRecognition::ExtractPixels(const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startPixelCoordinates,
					const FVector2D& featureImageDimensions);*/
			}
		}

		if (fileContents[0] == "SpeachBubble")
		{
			TArray<FColor> pixels{};
			bool bIsValid = false;
			int32 width = 0;
			int32 height = 0;
			if (UVictoryBPFunctionLibrary::Victory_LoadTexture2D_FromFile_Pixels(FPaths::ProjectSavedDir() + "ImageRecognition/" + "000Image.png", EJoyImageFormats::PNG, bIsValid, width, height, pixels))
			{

			}
			else if (!pixels.Num())
			{
				return false;
			}

			bool bBothVecsValid = false;
			FVector2D startLoc;
			FVector2D endLoc;
			bBothVecsValid = MyStringToVector2DConv(fileContents[2], startLoc);
			bBothVecsValid = MyStringToVector2DConv(fileContents[3], endLoc);

			FBox2D bounds;
			if (bBothVecsValid)
			{
				bounds = FBox2D(startLoc, endLoc);
			}
			else
			{
				bounds = FBox2D(FVector2D::ZeroVector, FVector2D(width, height));
			}

			const FVector2D imageDims = FVector2D(width, height);
			TArray<FString> sentences = ATextImageSearchActor::BuildSentenceFromImages(pixels, imageDims, nullptr, true, FColor::White, bounds);

			TArray<FString> tempStringArray{};
			TArray<FDoubleArray> weights{};
			UWordToVecFormatting::GetFormattedData("SentenceSeqModelWeights.txt", weights, tempStringArray, EFormattingType::Numbers);
			
			TArray<FDoubleArray> tempBiases{};
			UWordToVecFormatting::GetFormattedData("SentenceSeqModelBiases.txt", tempBiases, tempStringArray, EFormattingType::Numbers);
			const TArray<double> biases = tempBiases[0].Doubles;

			for (const FString& sentence : sentences)
			{
				USentenceRecognition::Predict(sentence, weights, biases);
			}
		}
	}

	return false;
}

bool APythonCommunictionsActor::MyStringToVector2DConv(const FString& IN_String, FVector2D& OUT_Vector)
{
	FString modifiedString = IN_String.Replace(TEXT("("), TEXT(""));
	modifiedString = modifiedString.Replace(TEXT(")"), TEXT(""));
	modifiedString = modifiedString.Replace(TEXT(" "), TEXT(""));

	TArray<FString> splitString{};
	modifiedString.ParseIntoArray(splitString, TEXT(","));
	if (splitString.Num() == 2)
	{
		float x = UKismetStringLibrary::Conv_StringToFloat(splitString[0]);
		float y = UKismetStringLibrary::Conv_StringToFloat(splitString[1]);
		OUT_Vector = FVector2D(x, y);
		return true;
	}

	return false;
}
