// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "MyAIPerceptionComponent.generated.h"

struct FAIStimulus;

UCLASS(ClassGroup = CUSTOM_AI, HideCategories = (Activation, Collision), meta = (BlueprintSpawnableComponent))
class PYTHON_API UMyAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
public:
	
};
