// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldStateComponent.h"
#include "DialogInformation.h"

float FAssociationMultipliers::GetCurveValue(const int32& featureIndex)
{
	if (UFactDictionaryPair* feature = Features[featureIndex])
	{
		FName timeAsFname = feature->GetComparisionValue(feature->Key);
		float time = UKismetStringLibrary::Conv_StringToFloat(timeAsFname.ToString());
		float value = Multipliers[featureIndex].GetRichCurve()->Eval(time);		
		return value;
	}

	return 0.0f; // Not sure about this as the default...?
}
