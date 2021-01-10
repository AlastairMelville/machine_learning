// Fill out your copyright notice in the Description page of Project Settings.

#include "RL_Test_Pawn.h"
#include "ContextAwareDialogueSystem/Classes/ContextualGameState.h"
#include "MemoryReadWrite.h"
#include "Kismet/KismetStringLibrary.h"
#include "Paths.h"
#include "ObservedActorInfo.h"


//static TArray<FKeyDataTypeMap> TEST_StaticContextDictionary = TArray<FKeyDataTypeMap>
//{
//	FKeyDataTypeMap(EKey::None, EDataType::None, TEXT("")),
//	FKeyDataTypeMap(EKey::Speaker, EDataType::Name, TEXT("")),
//	FKeyDataTypeMap(EKey::Listener, EDataType::Name, TEXT("")),
//	FKeyDataTypeMap(EKey::Listener_b, EDataType::Name, TEXT("")),
//	FKeyDataTypeMap(EKey::Listener_c, EDataType::Name, TEXT("")),
//	FKeyDataTypeMap(EKey::Health, EDataType::Float, TEXT("100")),
//	FKeyDataTypeMap(EKey::Ammo, EDataType::Int, TEXT("")),
//	FKeyDataTypeMap(EKey::LastSeenTime, EDataType::Float, TEXT("")),
//	FKeyDataTypeMap(EKey::EnemyCount, EDataType::Int, TEXT("")),
//	FKeyDataTypeMap(EKey::Count, EDataType::None, TEXT("")) UMETA(Hidden)
//};


int64 FCurrentSituation::GetEnumAtSituationIndex(const uint32& situationIndex)
{
	UEnum* globalSituationEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGlobalSituationElement"), true);
	if (globalSituationEnum)
	{
		FString localEnumName = globalSituationEnum->GetNameByIndex(situationIndex).ToString();
		if (localEnumName != "NAME_None")
		{
			UEnum* localSituationEnum = FindObject<UEnum>(ANY_PACKAGE, *localEnumName, true);
			if (localSituationEnum)
			{
				int64 localEnumValue = localSituationEnum->GetValueByIndex(Situation[situationIndex]);
				if (localEnumValue != INDEX_NONE)
				{
					return localEnumValue;
				}
			}
		}
	}

	return INDEX_NONE;
}


void UTEST_State::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTEST_State, bMakeManualSave))
		{
			if (bMakeManualSave)
			{
				FString filepath;
				if (ARL_Test_Pawn::GetMemoryFilePath(filepath, DictionaryState))
				{
					FObservedActor placeholder;
					UMemoryReadWrite::SaveToFilePath(filepath, Actions, placeholder);
				}
				
				bMakeManualSave = false;
			}
		}
	}
}

TArray<ETest_Relationship> ARL_Test_Pawn::PairToLookFor = TArray<ETest_Relationship>();
UTEST_State* ARL_Test_Pawn::FoundState = nullptr;

ARL_Test_Pawn::ARL_Test_Pawn()
	: StatePtrPairs{}
{
	//PrimaryActorTick.bCanEverTick = true;
}

bool ARL_Test_Pawn::GetMemoryFilePath(FString& OUT_filepath, const TArray<FTEST_DictionaryFact>& dictionaryFacts)
{
	FString filename;
	for (uint8 index = 0; index < /*EKey::Count*/ 9; ++index)
	{
		filename += UKismetStringLibrary::Conv_IntToString(dictionaryFacts[index].EnumElementIdx);
	}

	FString filepath = FPaths::ProjectSavedDir() + "MemoriesOfCharacterA/" + filename + ".txt";
	if (FPaths::FileExists(filepath))
	{
		OUT_filepath = filepath;
		return true;
	}

	return false;
}

EAction ARL_Test_Pawn::ChooseAction()
{
	EAction chosenAction = EAction::Duck;

	FString filepath;
	TArray<FTEST_Action> actions{FTEST_Action()};
	if (GetMemoryFilePath(filepath, TEST_StaticDictionary))
	{
		FObservedActor placeholder;
		if (UMemoryReadWrite::LoadFromFilePath(filepath, actions, placeholder))
		{
			if (actions.Num())
			{

				actions.Sort();
				chosenAction = actions[0].Action;
				UE_LOG(LogTemp, Warning, TEXT("chosenAction: %i"), (uint8)chosenAction);
			}
		}
	}

	return chosenAction;

	//FStatePtrPair* pair = StatePtrPairs.FindByPredicate([this](FStatePtrPair& item)
	//{
	//	if (const UTEST_State* state = item.Get())
	//	{
	//		const int32 numFacts = state->DictionaryState.Num();
	//		for (int32 index = 0; index < numFacts; ++index)
	//		{
	//			FTEST_DictionaryFact factToMatch = state->DictionaryState[index];

	//			// 
	//			FTEST_DictionaryFact* fact = TEST_StaticDictionary.FindByPredicate([factToMatch](FTEST_DictionaryFact& item)
	//			{
	//				if (factToMatch.Key != item.Key)
	//				{
	//					// If this is not the dictionary entry that you want to check the value of, then don't bother going any furtehr
	//					return false;
	//				}

	//				UEnum* selectedEnumType = FindObject<UEnum>(ANY_PACKAGE, *factToMatch.EnumName, true);
	//				if (selectedEnumType)
	//				{
	//					return item.Value == selectedEnumType->GetValueByIndex(factToMatch.EnumElementIdx);
	//				}
	//				else
	//				{
	//					return false;
	//				}

	//				//return factToMatch.Value == item.Value;
	//			});
	//			if (!fact)
	//			{
	//				// Failed to confirm anything
	//				return false;
	//			}
	//		}
	//	}

	//	return true; // PairToLookFor.Key == item.Get()->Speaker && PairToLookFor.Value == item.Get()->Listener;
	//});
	//if (pair)
	//{
	//	if (pair->Get()->Actions.Num())
	//	{
	//		FoundState = pair->Get();

	//		pair->Get()->Actions.Sort();
	//		EAction chosenAction;
	//		chosenAction = pair->Get()->Actions[0].Action;
	//		UE_LOG(LogTemp, Warning, TEXT("chosenAction: %i"), (uint8)chosenAction);
	//		return;
	//	}
	//}
}

EAction ARL_Test_Pawn::cg_SayTheSentence(FString sentence)
{
	FObservedActor combinedActorInfo;

	if (sentence == "Speaker is Fred")
	{
		const TArray<EKey> keys = TArray<EKey>{ EKey::Speaker, EKey::Weapon };

		for (uint8 index = 0; index < keys.Num(); ++index)
		{
			EKey key = keys[index];
			FTEST_DictionaryFact* pair = TEST_StaticDictionary.FindByPredicate([key](const FTEST_DictionaryFact& item)
			{
				return key == item.Key;
			});
			if (pair)
			{
				UEnum* selectedEnumType = FindObject<UEnum>(ANY_PACKAGE, *pair->EnumName, true);
				if (selectedEnumType)
				{
					pair->Value = selectedEnumType->GetValueByIndex(pair->EnumElementIdx);
					UE_LOG(LogTemp, Warning, TEXT("pair->Value: %i"), pair->Value);
				}

				//pair->Value = ETest_Relationship::Friend;
			}
		}
	}
	else if (sentence == "Dave is angry")
	{
		FString filepath = FPaths::ProjectSavedDir() + "MemoriesOfCharacterA/_DescriptiveActorModels/Dave/Characters/" + "Dave";

		TArray<FTEST_Action> placeholderActions{};
		if (UMemoryReadWrite::LoadFromFilePath(filepath, placeholderActions, combinedActorInfo))
		{
			// Initiating emotional reactions based on how the person is acting compared to their usual self
			float chance = combinedActorInfo.ExistingObservedActorInfo.CalculateChanceOfEmotion(EObservedEmotionalState::Unknown);

			const float trustInSpeaker = 0.65f; // TEMP constant
			chance *= (1.0f + trustInSpeaker);
			if (chance > 0.5f)
			{
				UE_LOG(LogTemp, Warning, TEXT("pair->Value: %f"), chance);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("pair->Value: %f"), chance);
			}
		}
	}
	else if (sentence == "Listener is Dave")
	{
		EKey key = EKey::Listener;
		FTEST_DictionaryFact* pair = TEST_StaticDictionary.FindByPredicate([key](const FTEST_DictionaryFact& item)
		{
			return key == item.Key;
		});
		if (pair)
		{
			//pair->Value = TEXT("Dave");
			pair->Value = (int64)ETest_Relationship::Friend;
			//PairToLookFor = TPair<ETest_Relationship, ETest_Relationship>{ ETest_Relationship::None, ETest_Relationship::Friend };
		}
	}
	else
	{
	}
	
	return ChooseAction();
}

void ARL_Test_Pawn::RespondToAction(float score)
{
	if (FoundState)
	{
		FoundState->Actions[0].PerceivedReward += score;
	}
}
