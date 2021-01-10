
#include "source/Engine/classes/GameFramework/Actor.h"
#include "source/Engine/classes/Engine/Level.h"

void AActor::BeginPlay()
{
	RegisterAllActorTickFunctions(true, false);
}

void AActor::Tick(float InDeltaTime)
{

}

void AActor::RegisterAllActorTickFunctions(bool bRegister, bool bDoComponents)
{
	if (bRegister)
	{
		RegisterActorTickFunctions(bRegister);
	}
	else
	{

	}
}

void AActor::RegisterActorTickFunctions(bool bRegister)
{
	if (bRegister)
	{
		if (PrimaryActorTick.bCanEverTick)
		{
			PrimaryActorTick.Target = this;
			//PrimaryActorTick.SetTickFunctionEnable(PrimaryActorTick.bStartWithTickEnabled || PrimaryActorTick.IsTickFunctionEnabled());
			PrimaryActorTick.RegisterTickFunction(GetLevel());
		}
	}
	else
	{

	}
}

ULevel* AActor::GetLevel() const
{
	return dynamic_cast<ULevel*>(Outer);
}
