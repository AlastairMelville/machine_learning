// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldUnderstandingComponent.h"
#include "EngineUtils.h"
#include "ContextualGameState.h"
#include "DialogInformation.h"
#include "WorldStateComponent.h"

UWorldUnderstandingComponent::UWorldUnderstandingComponent()
	: WorkingMemory{}
	, LatestWorkingMemoryIndex(0)
{
	PrimaryComponentTick.bCanEverTick = true;

	WorkingMemory.SetNum(MaxNumWorkingMemories);
}

void UWorldUnderstandingComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UWorldUnderstandingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UWorldUnderstandingComponent::FindReferencedActor(const FString& actorType)
{
	// Find the object that the speaker was refering to in their sentence

	int32 index = ReferencedActors.Add(FActorReference());

	// First, see if the sentence contains "you", "him", "her" etc. and search which actor is linked to that word in the relevant Fact Dictionary
	// TODO: 


	// Second, if the speaker was a player, try to find the object in the array of actors that the player looked at when writing the sentence
	for (int32 i = 0; i < ActorsThePlayerLookedAt.Num(); ++i)
	{

	}

	// Thirdly, search the list of actors that are currently important to the character (or player)
	for (int32 i = 0; i < ImportantActors.Num(); ++i)
	{

	}


	// Lastly, if you haven't found the object being refered to yet, search all actors of it's type and try to match it's name to one of them	
	for (TActorIterator<AActor> Itr(GetWorld()); Itr; ++Itr)
	{
		if (AActor* actor = *Itr)
		{
			int32 tagIndex = INDEX_NONE;
			if (actor->Tags.Find(FName(*actorType), tagIndex))
			{
				// Change the appropriate dictionary with what was spoken
				EKey key = EKey::EnemyCount;
				FKeyDataTypeMap* pair = StaticContextDictionary.FindByPredicate([key](const FKeyDataTypeMap& item)
				{
					return key == item.Key;
				});
				if (pair)
				{
					pair->Value = TEXT("");
				}

				// Check if the new dictionary state matches any saved WorldStates

				// TEMP: Auto find the below WorldState
				TSoftObjectPtr<UWorldStateComponent> worldStatePtr;
				worldStatePtr = FStringAssetReference(TEXT("/Game/z_DA_WorldState.z_DA_WorldState"));
				worldStatePtr.LoadSynchronous();
				if (const UWorldStateComponent* worldState = worldStatePtr.Get())
				{
					if (const UNormSituationalResponse* response = worldState->NormSituationalResponse)
					{
						float emotionalEffect = 0.0f;
						const float minEmotionalEffectForSomething = 1.0f;
						const TArray<FEmotionalAssociation>& emotionalAssociations = response->BaseEmotionalAssociation;
						for (uint8 emIdx = 0; emIdx < emotionalAssociations.Num(); ++emIdx)
						{

						}

						// If the potential emotional reaction isn't enough to make a decision/reaction etc. then check the connected WorldStates
						if (emotionalEffect < minEmotionalEffectForSomething)
						{
							const TArray<TSoftObjectPtr<UWorldStateComponent>>& relatedWorldStates = worldState->RelatedStates;
							for (uint8 index = 0; index < relatedWorldStates.Num(); ++index)
							{

							}
						}
					}
				}



				
				if (ReferencedActorNames.IsValidIndex(index))
				{
					if (actor->GetName() == ReferencedActorNames[index])
					{
						ReferencedActors[index].Actor = actor;
					}
				}
			}
		}
	}

	return false;
}

bool UWorldUnderstandingComponent::GetReferredToObject(const TArray<FActorReference>& referencedActors, uint8 OUT_index)
{
	for (uint8 index = 0; index < referencedActors.Num(); ++index)
	{
		if (referencedActors[index].ObjectFeature == EObjectFeature::Object)
		{
			OUT_index = index;
			return true;
		}
	}

	return false;
}

bool UWorldUnderstandingComponent::QuestionAsked(EObjectFeature infoTypeToRead, UWorldUnderstandingComponent* characterAsked)
{
	switch (infoTypeToRead)
	{
	case EObjectFeature::ObjectCurrentHealth:
	{
		return characterAsked->AskedIfFeelingWell(nullptr);
		break;
	}
	default:
	{
		break;
	}
	}

	return false;
}

bool UWorldUnderstandingComponent::StatementReceived(EObjectFeature infoTypeToWrite, UWorldUnderstandingComponent* characterTold)
{
	switch (infoTypeToWrite)
	{
	case EObjectFeature::ObjectCurrentHealth:
	{

		break;
	}
	default:
	{
		break;
	}
	}

	return false;
}

void UWorldUnderstandingComponent::UpdateDictionary()
{

}

void UWorldUnderstandingComponent::AddToMemory(USentenceLabel* sentenceMeaning)
{
	// TODO: Utulise memory chunking

	WorkingMemory[LatestWorkingMemoryIndex++] = sentenceMeaning;

	if (LatestWorkingMemoryIndex >= MaxNumWorkingMemories)
	{
		LatestWorkingMemoryIndex = 0;
	}
}

bool UWorldUnderstandingComponent::GetMemoryFile(FString& OUT_filepath)
{
	// Get file with latest time stamp if no date was given in sentence (GetTimeStamp())


	return false;
}

void AWorldUnderstandingTester::ec_SaySentence(FString sentence)
{
	FindComponentByClass<UWorldUnderstandingComponent>()->FindReferencedActor(sentence);
}

bool USentenceLabel::GetFloatByName(UObject * Target, FName VarName, float &outFloat)
{
	if (Target) //make sure Target was set in blueprints. 
	{
		float FoundFloat;
		UFloatProperty* FloatProp = FindField<UFloatProperty>(Target->GetClass(), VarName);  // try to find float property in Target named VarName
		if (FloatProp) //if we found variable
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  // get the value from FloatProp
			outFloat = FoundFloat;  // return float
			return true; // we can return
		}
	}
	return false; // we haven't found variable return false
}
