// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonCharacter.h"
#include "MemoryReadWrite.h"
#include "FileHelper.h"
#include "RL_Test_Pawn.h"

APythonCharacter::APythonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APythonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind to your own actions, so that you can keep your own actions documented for tracking habits and self-awareness
	if (!ChosenActionDelegate.IsBoundToObject(this))
	{
		ChosenActionDelegate.AddUObject(this, &APythonCharacter::OnObservedAction);
	}
}

void APythonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APythonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

ECharacterState APythonCharacter::PercieveCharacterState()
{
	// TODO: Make some barriers to be able to get the character's state
	if (true)
	{
		GetCharacterState();
	}

	return ECharacterState::CannotTell;
}

bool APythonCharacter::UpdateObservedActors()
{
	bool bEditMade = false;

	int32 numActors = ObservedActors.Num();
	for (int32 index = 0; index < numActors; ++index)
	{
		if (ObservedActors[index].Stimulus.GetAge() > ObservationMaxAge)
		{
			ObservedActors.RemoveAtSwap(index);

			if (!bEditMade)
			{
				bEditMade = true;
			}
		}
	}

	return bEditMade;
}

bool APythonCharacter::QueryObservedActor(const AActor* actor, const EObservedActorInfo& infoToQuery, bool& OUT_bFound, const FAIStimulus& infoToMatch)
{
	FObservedActor* foundItem = ObservedActors.FindByPredicate([actor](const FObservedActor& item) 
	{
		return item.Actor == actor;
	});
	if (foundItem)
	{
		OUT_bFound = true;

		switch (infoToQuery)
		{
		case EObservedActorInfo::Age:
		{
			float diff = FMath::Abs(foundItem->Stimulus.GetAge() - infoToMatch.GetAge());
			if (diff < 0.2f)
			{
				return true;
			}

			break;
		}
		case EObservedActorInfo::ExpirationAge:
		{

			break;
		}
		case EObservedActorInfo::StimulusStrength:
		{

			break;
		}
		case EObservedActorInfo::StimulusLocation:
		{
			float distance = FVector::Dist(foundItem->Stimulus.StimulusLocation, infoToMatch.StimulusLocation);
			if (distance < 500.0f)
			{
				return true;
			}
			break;
		}
		case EObservedActorInfo::ReceiverLocation:
		{

			break;
		}
		default:
		{
			break;
		}
		}
	}
		
	return false;
}

void APythonCharacter::OnReceivedTargetUpdate(const FObservedActor& observedActorInfo)
{
	AActor* actor = observedActorInfo.Actor;
	if (!actor)
	{
		return;
	}

	if (SpareAttentionLevel == EAttentionLevel::NoSpareAttention)
	{
		// Not paying enough attention to notice anything anyway, so exit here
		return;
	}

	FObservedActor* foundActor = ObservedActors.FindByPredicate([actor](const FObservedActor& item)
	{
		return item.Actor == actor;
	});
	if (!foundActor)
	{
		int32 index = ObservedActors.Add(FObservedActor(observedActorInfo.Actor));

		// Now try to find the new FObservedActor
		foundActor = ObservedActors.FindByPredicate([actor](const FObservedActor& item)
		{
			return item.Actor == actor;
		});
	}

	if (APythonCharacter* character = Cast<APythonCharacter>(actor))
	{
		// Attempt to observe character's emotion //
		// Ensure that you can see the character's face (or an expressive animation) before getting her/his observable emotion (may not be what they are truly feeling)
		float yourYaw = GetActorRotation().Yaw;
		float theirYaw = character->GetActorRotation().Yaw;
		float relativeYaw = yourYaw + FMath::Abs(theirYaw) - 180.0f;
		if (relativeYaw < 0.0f)
		{
			relativeYaw += 360.0f;
		}

		float allowableOffset = 45.0f;
		if (relativeYaw <= allowableOffset || relativeYaw >= (360.0f - allowableOffset))
		{
			// If you are facing eachother (within 45 degrees), 
			foundActor->ObservedEmotionalState = observedActorInfo.ObservedEmotionalState;
		}

		FString characterName = character->GetCharacterName().ToString();

		// Save/append data to file
		FString filepath = FPaths::ProjectSavedDir() + "MemoriesOfCharacterA/_DescriptiveActorModels/Characters/" + characterName;

		TArray<FTEST_Action> placeholderActions{};
		FObservedActor combinedActorInfo = FObservedActor(foundActor->Actor);
		if (UMemoryReadWrite::LoadFromFilePath(filepath, placeholderActions, combinedActorInfo))
		{
			
		}
		else
		{
			// Create a new text file
			FFileHelper::SaveStringToFile("", *filepath); // auto Ar = TUniquePtr<FArchive>( FileManager->CreateFileWriter( Filename, WriteFlags ) );
		}

		// Append existing saved data to new saved data, and then save it all back to the file
		if (combinedActorInfo.Actor)
		{
			// Add observed emotional state to saved data
			bool emotionaExisted = combinedActorInfo.ExistingObservedActorInfo.AddToExistingEmotionalModel(foundActor->ObservedEmotionalState);

			//IFileManager::Move(); // Moves a memory file


			TArray<FTEST_Action> placeholderActions{};
			combinedActorInfo.ActorName = characterName;
			combinedActorInfo.TotalDataNum = 18;
			combinedActorInfo.ObservedEmotionalState = EObservedEmotionalState::Angry;
			UMemoryReadWrite::SaveToFilePath(filepath, placeholderActions, combinedActorInfo /**foundActor*/);

			// Continue observing the character and track the actions she/he takes
			if (!character->ChosenActionDelegate.IsBoundToObject(this))
			{
				character->ChosenActionDelegate.AddUObject(this, &APythonCharacter::OnObservedAction);
			}

			/*TArray<FTEST_Action> placeholderActionsTwo{};
			FObservedActor placeholderActorInfo;
			UMemoryReadWrite::LoadFromFilePath(filepath, placeholderActionsTwo, placeholderActorInfo);
			UE_LOG(LogTemp, Log, TEXT(""));*/
		}
	}
	/*else if ()
	{

	}*/
	else
	{
		// Is just a standard AActor (or an actor type that we don't care about in this situation)

	}
}

void APythonCharacter::cg_CharacterSayTheSentence(FString sentence)
{
	if (ObservedActors.Num())
	{
		EAction chosenAction = ARL_Test_Pawn::cg_SayTheSentence(sentence);

		if (APythonCharacter* observedCharacter = Cast<APythonCharacter>(ObservedActors[0].Actor))
		{
			if (observedCharacter->ChosenActionDelegate.IsBound())
			{
				observedCharacter->ChosenActionDelegate.Broadcast(chosenAction, this);
			}
		}
	}
}

void APythonCharacter::OnObservedAction(EAction chosenAction, APythonCharacter* character)
{
	float personalValueOfAction = 0.0f;
	FString situationFilepath;
	TArray<FTEST_Action> actions{ FTEST_Action() };
	if (ARL_Test_Pawn::GetMemoryFilePath(situationFilepath, TEST_StaticDictionary)) // TODO: Use static dictionary local to this character - could differ, thus creating confusion and other reactions etc.
	{
		FObservedActor placeholder;
		if (UMemoryReadWrite::LoadFromFilePath(situationFilepath, actions, placeholder))
		{
			if (actions.Num())
			{
				// Judge the action taken by the other based on your own experiences - the scores you have attached to each action in the specific situation
				FTEST_Action* actionChosenByOther = actions.FindByPredicate([chosenAction] (const FTEST_Action& item)
				{
					return item.Action == chosenAction;
				});
				if (actionChosenByOther)
				{
					personalValueOfAction = actionChosenByOther->PerceivedReward;
				}
				else
				{
					// Default reaction??
				}
			}
		}
	}
	// TODO: react based on the value of personalValueOfAction (e.g. you act confused, as you don't know why this person would choosen an action that has such a low score in the given situation - but your score could greatly differ from their's)
	

	FString situation = situationFilepath.Len() ? (situationFilepath.Right(13)).Replace(TEXT(".txt"), TEXT("")) : "100000000";
	FString characterName = character->GetCharacterName().ToString();
	FString filepath = FPaths::ProjectSavedDir() + "MemoriesOfCharacterA/_DescriptiveActorModels/" + CharacterName.ToString() + "/Characters/" + characterName;

	TArray<FTEST_Action> placeholderActions{};
	FObservedActor combinedActorInfo;
	if (UMemoryReadWrite::LoadFromFilePath(filepath, placeholderActions, combinedActorInfo))
	{
		combinedActorInfo.ExistingObservedActorInfo.AddToExistingActionModel(situation, chosenAction);

		UMemoryReadWrite::SaveToFilePath(filepath, placeholderActions, combinedActorInfo);
	}
}

void APythonCharacter::cg_CharacterMakeDecision()
{
	if (ChosenActionDelegate.IsBound())
	{
		ChosenActionDelegate.Broadcast(EAction::Duck, this);
	}
}
