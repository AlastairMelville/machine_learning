// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PixelCapture.generated.h"

struct FDoubleArray;

UCLASS()
class PYTHON_API APixelCapture : public AActor
{
	GENERATED_BODY()
	
public:	
	APixelCapture();

	virtual void Tick(float DeltaTime) override;

	static bool CapturePixels();

	static bool TrimPixels(TArray<FDoubleArray>& OUT_pixelMatrix);

	static bool ExportPixels(const TArray<FDoubleArray>& pixelMatrix);
	
protected:
	virtual void BeginPlay() override;
};
