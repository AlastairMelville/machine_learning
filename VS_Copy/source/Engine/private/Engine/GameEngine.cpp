
#include <pybind11/pybind11.h>

#include "source/Engine/classes/Engine/GameEngine.h"
#include "source/Engine/classes/Engine/World.h"
#include "source/Engine/classes/Engine/GameInstance.h"


void UGameEngine::Init()
{
	// Create a WorldContext for the editor to use and create an initially empty world.
	FWorldContext& InitialWorldContext = CreateNewWorldContext();
	InitialWorldContext.SetCurrentWorld(UWorld::CreateWorld());
}

void UGameEngine::Tick(float InDeltaTime)
{
	// kick off a "Play From Here" if we got one
	static bool bIsPlayWorldQueued = true;
	if (bIsPlayWorldQueued)
	{
		StartQueuedPlayMapRequest();
		bIsPlayWorldQueued = false;
	}

	if (World)
	{
		//World->Tick(InDeltaTime);
	}
}

void UGameEngine::StartQueuedPlayMapRequest()
{
	CreateGameInstance();
}

void UGameEngine::CreateGameInstance()
{
	//std::unique_ptr<UGameInstance> GameInstance = NewObject<UGameInstance>(new UGameInstance, this);
	//GameInstance->Initialise();
}

FWorldContext& UGameEngine::CreateNewWorldContext()
{
	FWorldContext* NewWorldContext = new FWorldContext;
	WorldList.push_back(NewWorldContext);

	return *NewWorldContext;
}

FWorldContext* UGameEngine::GetWorldContext()
{
	return WorldList.size() > 0 ? WorldList[0] : nullptr;
}


namespace py = pybind11;

void Init_GameEngine(py::module& m)
{
	/*py::class_<UGameEngine>(m, "UGameEngine")
		.def(py::init<>())
		.def("Tick", &UGameEngine::Tick);*/
}
