// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectRecognitionBase.h"
#include "GameFramework/Actor.h"
#include "ImageRecognition.generated.h"

UENUM(Blueprintable)
enum class ELabel : uint8
{
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	COUNT UMETA(Hidden)
};

struct FStringArray;
struct FFloatArray;

USTRUCT(BlueprintType)
struct PYTHON_API FLabelWeight
{
	GENERATED_BODY()

	FLabelWeight()
		: Weights()
	{
		Weights.SetNum(int32(ELabel::COUNT));
	}

	FLabelWeight(TArray<float> weights)
		: Weights(weights)
	{
		while (Weights.Num() < int32(ELabel::COUNT))
		{
			Weights.Add(0.0f);
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Weights;
};

class UTexture2D;
class TextureRenderTarget2D;
class AStaticMeshActor;
class UMaterial;
struct FPredictionCallback;

UCLASS()
class PYTHON_API UImageRecognition : public UObjectRecognitionBase
{
	GENERATED_BODY()
	
public:
	friend class ATester;

	static FPredictionCallback Predict(const TArray<double> imagePixelValues, const TArray<FDoubleArray> weights, const TArray<double> biases,
		const TArray<int32> labels /* = TArray<int32>() */);

	UFUNCTION(BlueprintCallable)
	static bool GetPixelsArrayFromT2D(UTexture2D* texture, int32& textureWidth, int32& textureHeight, TArray<FLinearColor>& pixelArray, 
		TArray<float> greyscaleArray, bool bConvertToGreyscale = true);

	UFUNCTION(BlueprintCallable)
	static float ConvertPixelToGreyscale(const FLinearColor& linearPixel, const FColor& colorPixel);

	UFUNCTION(BlueprintCallable)
	static TArray<float> ConvertPixelArrayToGreyscale(const TArray<FLinearColor>& linearPixelArray, const TArray<FColor>& colorPixelArray);

public:
	TArray<float> Evaluate(TArray<float>& pixelValueArray, TArray<FLabelWeight>& pixelWeightArray);

	bool FindFirstPixelInImage(const TArray<double>& pixels, const FVector2D& pixelsDimensions, const TArray<FDoubleArray>& pixelWeights, uint32& OUT_pixelLocation);

	bool PixelsMatch(const double& imagePixel, const double& pixelWeight);

	/* Extract the pixels that will be compared against the weights for the relevant text image features. */
	TArray<double> ExtractPixels(const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startPixelCoordinates, const FVector2D& featureImageDimensions);

	FVector2D FindEndOfWord(const TArray<double>& pixels, const FVector2D& startLocation, const FVector2D& dimensions);

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere)
	uint8 bPredict : 1;

	UPROPERTY(EditAnywhere)
	FString ImagePixelsFilename;

	UPROPERTY(EditAnywhere)
	FString WeightsFilename;

	UPROPERTY(EditAnywhere)
	FString BiasesFilename;

	UPROPERTY(EditAnywhere)
	uint8 bFindFirstPixel : 1;

	UPROPERTY(VisibleAnywhere)
	double ClosestMatch = 1.0;

	UPROPERTY(VisibleAnywhere)
	uint32 ClosestMatchingPixel;

	UPROPERTY(EditAnywhere)
	uint8 bGetPercentRelevantPixelsInWordImage : 1;

	UPROPERTY(EditAnywhere)
	FString FullFilepath;

	bool FormatData();
#endif // WITH_EDITOR
};

UCLASS()
class PYTHON_API ATester : public AActor
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Stuff();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTextureRenderTarget2D* RenderTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTexture* RenderTargetTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTexture* MediaTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTexture2D* NewTexture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AStaticMeshActor* ActorToPutNewTextureOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UMaterial* ActorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InstanceEditable = "true"))
	UTexture2D* new2DTexture;
};
