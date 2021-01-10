// Fill out your copyright notice in the Description page of Project Settings.

#include "ObservedActorInfo.h"


bool FExistingObservedActorInfo::AddToExistingEmotionalModel(const EObservedEmotionalState& observedEmotionalState)
{
	uint32* value = ObservedEmotionalStates.Find(observedEmotionalState);
	if (value)
	{
		++*value;
		++NumEmotionalStateObservations;
		return true;
	}
	else
	{
		uint32& newValue = ObservedEmotionalStates.Add(observedEmotionalState);
		++newValue;
		++NumEmotionalStateObservations;
		return false;
	}
}

float FExistingObservedActorInfo::CalculateChanceOfEmotion(const EObservedEmotionalState& potentialEmotionalState)
{
	uint32* numOccurances = ObservedEmotionalStates.Find(potentialEmotionalState);
	if (numOccurances && *numOccurances > 0)
	{
		return (float)*numOccurances / (float)NumEmotionalStateObservations;
	}

	return 0.0f;
}

bool FExistingObservedActorInfo::AddToExistingActionModel(const FString& observedSituation, const EAction observedAction)
{
	FObservedAction* action = nullptr;
	bool bAlreadyExisted = false;

	FObservedActionContainer* relevantActions = ObservedActions.Find(observedSituation);
	if (relevantActions)
	{
		action = relevantActions->ObservedActions.FindByPredicate([observedAction](const FObservedAction& item)
		{
			return item.Action == observedAction;
		});
		if (action)
		{
			++action->NumTimesChosen;
			++NumChosenActionsObserved;
			bAlreadyExisted = true;
		}
	}
	else
	{
		FObservedActionContainer& relevantActions = ObservedActions.Add(observedSituation, FObservedActionContainer(observedAction));
		action = relevantActions.ObservedActions.FindByPredicate([observedAction](const FObservedAction& item)
		{
			return item.Action == observedAction;
		});
		if (action)
		{
			++action->NumTimesChosen;
			++NumChosenActionsObserved;
		}
	}

	if (action)
	{
		UpdateObservedPersonality(action->Action);
	}

	return bAlreadyExisted;
}

float FExistingObservedActorInfo::CalculateChanceOfAction(const FString& observedSituation, const EAction observedAction)
{
	FObservedActionContainer* relevantActions = ObservedActions.Find(observedSituation);
	if (relevantActions && relevantActions->ObservedActions.Num())
	{
		FObservedAction* action = relevantActions->ObservedActions.FindByPredicate([observedAction](const FObservedAction& item)
		{
			return item.Action == observedAction;
		});
		if (action)
		{
			return (float)action->NumTimesChosen / (float)NumChosenActionsObserved;
		}
	}

	return 0.0f;
}

static TArray<FActionPersonalityTraitPair> ActionPersonalityTraitMap = TArray<FActionPersonalityTraitPair>
{
	FActionPersonalityTraitPair(EAction::Duck, EPersonalityTrait::Emotionality),
	FActionPersonalityTraitPair(EAction::Jump, EPersonalityTrait::None),
	FActionPersonalityTraitPair(EAction::LieDown, EPersonalityTrait::None),
};

bool FExistingObservedActorInfo::UpdateObservedPersonality(const EAction observedAction)
{
	FActionPersonalityTraitPair* relatedTrait = ActionPersonalityTraitMap.FindByPredicate([observedAction](const FActionPersonalityTraitPair& item)
	{
		return item.Action == observedAction;
	});
	if (relatedTrait)
	{
		uint32* value = ObservedPersonality.ObservedPersonality.Find(relatedTrait->PersonalityTrait);
		if (value)
		{
			++*value;
			return true;
		}
	}

	return false;
}

uint16 FObservedActor::GetPercentageUncovered()
{
	uint16 currentNumData = 0;

	if (ObservedEmotionalState > EObservedEmotionalState::Unknown)
	{
		++currentNumData;
	}
	// TODO: Do this for many other data

	return currentNumData / TotalDataNum;
}
