
#include "source/Engine/classes/Engine/GameInstance.h"
#include "source/Engine/classes/Engine/World.h"

UGameInstance::UGameInstance()
{

}

void UGameInstance::Initialise()
{
	World = std::make_shared<UWorld>();
	World->InitWorld();
}
