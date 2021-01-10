// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObjectRecognitionBase.h"
#include "GameFramework/Actor.h"
#include "TextImageSearch.generated.h"


struct PYTHON_API LetterDim
{
	LetterDim()
	{}

	LetterDim(int16 startDiff, int16 endDiff)
		: StartDiff(startDiff)
		, EndDiff(endDiff)
	{}

	int16 StartDiff; // from j

	int16 EndDiff; // from j
};

UENUM(Blueprintable)
enum class ECapitalLetters : uint8
{
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
	Count = 26 UMETA(Hidden), Searching, Invalid,
};

UENUM(Blueprintable)
enum class EPunctuation : uint8
{
	FullStop, Comma,
	Count UMETA(Hidden)
};

UCLASS()
class PYTHON_API UTextImageSearch : public UObjectRecognitionBase
{
	GENERATED_BODY()

public:
	static bool FindNextWordStart(bool bIsCapitalLetter, const float& greyscaleSearchColour, const FVector2D& startLocation, const FVector2D& endLocation, const TArray<FDoubleArray>& pixelMatrix,
		const TArray<FDoubleArray>& weightsMatrix, FVector2D& OUT_pixelLocation, int32& OUT_pixelHeight, int8& bNewSentence);

	static bool CheckIfMissedPixels(const float& greyscaleSearchColour, const FVector2D& startLocation, const int32& numRowsToCheck, const TArray<FDoubleArray>& pixelMatrix, FVector2D& OUT_pixelLocation);

	static bool GetLetterPixelHeight(const float& greyscaleSearchColour, bool bIsCapitalLetter, const FVector2D& startLocation, const TArray<FDoubleArray>& pixelMatrix, int32& OUT_height);

	static bool DetermineFirstLetter(const FVector2D& wordStartLoc, const int32& fontSize, const TArray<FDoubleArray>& pixelMatrix, FString& letter, bool& bIsCapital);

	static bool FindNextSpace(const float& greyscaleSearchColour, bool bIsCapitalLetter, const FVector2D& startLocation, const int32& desiredMinSpaceSize, const int32& maxSpaceSize, const TArray<FDoubleArray>& pixelMatrix, FVector2D& OUT_pixelLocation);

	static bool FindFileLocationsForPotentialWords(const FString& startingLetter, const bool& bIsCapital, const uint8& wordLength, FString& OUT_files);

	static bool ExtractPixelWeightsFromFile(const FString& fullFilepath, TArray<FDoubleArray>& OUT_pixelWeights);

	static void ResizePixelMatrix(TArray<FDoubleArray>& pixelWeights, float scaleX, float scaleY);

	static void ResizePixelArray(TArray<float>& pixelWeights, int32 originalSizeX, int32 originalSizeY, float scaleX, float scaleY);

	static void ResizePixelArray(TArray<double>& pixelWeights, int32 originalSizeX, int32 originalSizeY, float scaleX, float scaleY);

	static void ResizePixelArray(TArray<FColor>& pixelWeights, int32 originalSizeX, int32 originalSizeY, int32 newWidth, int32 newHeight);

	static bool GetPixelsForWordPrediction(const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startLocation, const FVector2D& endLocation,
		TArray<double>& OUT_pixelArray, const int32& fixedColLen = 0);

	static bool DetectPunctuation(const float& greyscaleSearchColour, const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startLocation, const FVector2D& endLocation,
		EPunctuation& OUT_punctuationFound, FVector2D& OUT_punctionStartLocation);

	static float Lerp(float s, float e, float t);

	static float Blerp(float c00, float c10, float c01, float c11, float tx, float ty);

	static void RemovePixelWeights(const FString& shortFilepath, TArray<FString>& pixelWeights, const FVector2D& originalPixelDimensions, 
		const int32& colsToTrim, const int32& rowsToTrim);

	// Debug Start
	static void SavePixelsToFile(const TArray<FDoubleArray>& pixelMatrix);
};

class UTextureRenderTarget2D;

UCLASS()
class PYTHON_API ATextImageSearchActor : public AActor
{
	GENERATED_BODY()

public:
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	static TArray<FString> BuildSentenceFromImages(TArray<FColor>& pixels, const FVector2D& imageDims, UTextureRenderTarget2D* renderTexture, bool bIsCapitalLetter, const FColor searchColour, const FBox2D bounds);

	static FPredictionCallback FetchWordFromImage(const TArray<FDoubleArray>& pixelMatrix, const float& greyscaleSearchColour, FVector2D& startLocation, int32& previousWordHeight,
		const FBox2D& bounds, bool bIsCapitalLetter, int8& OUT_foundSentenceEnd, bool& OUT_reachedTextEnd);

	UFUNCTION(BlueprintCallable)
	bool CapturePixels(const FString& filepath, bool bIsCapital, const int32& wordIndex);

	static bool ExportPixels(const FString& filepath, const TArray<double>& pixelArray, const int32& wordIndex, const int32& wordLength);

	UFUNCTION(BlueprintCallable)
	int32 SplitWordWeightsIntoSeperateFiles(const FString& wordPixelFilepath, const FString& weightFilepath);

	static bool ExtractWordWeights(const FString& fullFilepath, const TArray<FString>& fileContents, const int32& colIndex,
		const int32& wordLength, TArray<FString>& OUT_wordWeights);
	
	UFUNCTION(BlueprintCallable)
	void ImageResizeTest(float scaleX, float scaleY);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTextureRenderTarget2D* RenderTexture;

#if WITH_EDITOR
	UPROPERTY(EditAnywhere)
	uint32 bTrimPixelWeightsInFile : 1;

	UPROPERTY(EditAnywhere)
	FVector2D DimensionTrim;

	UPROPERTY(EditAnywhere)
	FString ShortFilePath;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
