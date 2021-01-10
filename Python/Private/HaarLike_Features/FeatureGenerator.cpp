// Fill out your copyright notice in the Description page of Project Settings.

#include "HaarLike_Features/FeatureGenerator.h"


UFeatureGenerator::UFeatureGenerator()
	: FeaturePixels{}
	, RowLength(1)
{

}

void UFeatureGenerator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFeatureGenerator, RowLength))
		{
			for (int32 index = 0; index < FeaturePixels.Num(); ++index)
			{
				int32 currentNum = FeaturePixels[index].Pixels.Num();
				int32 diff = RowLength - currentNum;

				for (int32 j = 0; j < diff; ++j)
				{
					if (diff > 0)
					{
						FeaturePixels[index].Pixels.Add(1.0f);
					}
					else if (diff < 0)
					{
						FeaturePixels.RemoveAt(FeaturePixels.Num() - 1);
					}
				}
			}
		}
	}
}
