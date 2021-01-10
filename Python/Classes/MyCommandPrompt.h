// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
//#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "MyCommandPrompt.generated.h"


UCLASS()
class PYTHON_API AMyCommandPrompt : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int32 RunQuery();

	UFUNCTION(BlueprintCallable)
	bool RunPythonScript();

	void OnProcessOutput(FString msg);

	void OnProcessCompleted(int32 exitCode);

protected:
	UPROPERTY(EditAnywhere)
	FString URL;

	UPROPERTY(EditAnywhere)
	FString CommandQuery;

	UPROPERTY(EditAnywhere)
	uint32 bShowCmdWindow : 1;
};
