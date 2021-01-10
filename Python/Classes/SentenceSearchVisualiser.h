// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SentenceSearchVisualiser.generated.h"

UCLASS()
class PYTHON_API ASentenceSearchVisualiser : public AActor
{
	GENERATED_BODY()
	
public:	
	ASentenceSearchVisualiser();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* New2DTexture;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> Pixels;
};
