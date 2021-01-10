// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UCanvas;
class UTexture2D;

UCLASS()
class PYTHON_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	UCanvas* GetCanvas() const;

	UFUNCTION(BlueprintCallable)
	UCanvas* GetDebugCanvas() const;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetCanvasDefaultTexture() const;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetDebugCanvasDefaultTexture() const;
};
