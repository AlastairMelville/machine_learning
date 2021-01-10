// Fill out your copyright notice in the Description page of Project Settings.

#include "ImportExport/ImageImporter.h"
#include "Paths.h"
#include "FileHelpers.h"
#include "Kismet/KismetStringLibrary.h"
#include "ImageRecognition.h"

UTexture2D* AImageImporter::ImportImagePixels(const FString& FullFilePath, EJoyImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, 
	TArray<FLinearColor>& OutPixels, const FString& SaveFilePath, ETextImageLabel Label)
{
	UTexture2D* texture = UVictoryBPFunctionLibrary::Victory_LoadTexture2D_FromFile_Pixels(FullFilePath, ImageFormat, IsValid, Width, Height, OutPixels);

	if (Label != ETextImageLabel::None)
	{
		PixelVectorLabelPairs.Add(FPixelVectorLabelPair(OutPixels, Label));
	}
	else
	{

	}	

	TArray<float> greyscalePixels = UImageRecognition::ConvertPixelArrayToGreyscale(OutPixels, TArray<FColor>());

	FString fullString;
	for (int32 index = 0; index < greyscalePixels.Num(); ++index)
	{
		FString newConv = UKismetStringLibrary::Conv_FloatToString(greyscalePixels[index] * (1.0f / 255.0f));
		fullString += newConv;

		if (index < greyscalePixels.Num() - 1)
		{
			// Don't do this for the last index
			fullString += ",";
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *fullString);


	fullString = fullString + "\n";
	for (int32 index = 0; index < FeatureLabelArray.Num(); ++index)
	{
		// 
		TArray<FString> splitFullFilePath{};
		FullFilePath.ParseIntoArray(splitFullFilePath, TEXT("\\"));
		FString filename = splitFullFilePath.Last();
		if (filename.Contains(FeatureLabelArray[index]))
		{
			fullString = fullString + UKismetStringLibrary::Conv_IntToString(index);
			break;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *fullString);

	if (FPaths::FileExists(SaveFilePath) && fullString.Len())
	{
		fullString = "\n" + fullString;
		FFileHelper::SaveStringToFile(fullString, *SaveFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}

	return texture;
}

bool AImageImporter::GetRenderTargetPixels(UTexture* renderTexture)
{
	if (RenderTargetTexture)
	{
		TArray<FColor> pixels{};
		check(IsInGameThread());

		FTextureRenderTargetResource* res = static_cast<FTextureRenderTargetResource*>(RenderTargetTexture->Resource);
		if (res)
		{
			res->ReadPixels(pixels);
			if (pixels.Num())
			{
				/*TArray<float> greyscalePixels = obj->ConvertPixelArrayToGreyscale(TArray<FLinearColor>(), pixels);
				if (greyscalePixels.Num())
				{

				}*/
			}
		}
	}

	return false;
}

#if WITH_EDITOR
void AImageImporter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AImageImporter, bImportImage))
		{
			if (bImportImage)
			{

				bImportImage = false;
			}
		}
	}
}
#endif // WITH_EDITOR
