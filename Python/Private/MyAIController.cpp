// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAIController.h"
#include "SceneBuildingBlocks/Classes/MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

AMyAIController::AMyAIController()
	: CachedPerception{}
{
}

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();
		
	UseBlackboard(BlackboardAsset, Blackboard);
	RunBehaviorTree(BehaviorTree);
}

void AMyAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (PawnAsCharacter)
	{
		if (Blackboard)
		{
			if (ACharacter* otherCharacter = Cast<ACharacter>(Blackboard->GetValueAsObject(TEXT("OtherCharacter"))))
			{
				///if (AActor* Target = PawnAsCharacter->GetTarget())
				{
					// We do have a target. Shamble toward it and attempt violence!
					FVector targetLocation = otherCharacter->GetActorLocation();
					FVector yourLocation = PawnAsCharacter->GetActorLocation();
					FVector DirectionToTarget = (targetLocation - yourLocation).GetSafeNormal2D();
					UE_LOG(LogTemp, Log, TEXT("DirectionToTarget: %s"), *DirectionToTarget.ToCompactString());
					FVector forwardVector = PawnAsCharacter->GetActorForwardVector();
					float DotToTarget = FVector::DotProduct(DirectionToTarget, forwardVector);
					FVector rightVector = PawnAsCharacter->GetActorRightVector();
					float SidewaysDotToTarget = FVector::DotProduct(DirectionToTarget, rightVector);
					float DeltaYawDesired = FMath::Atan2(SidewaysDotToTarget, DotToTarget); // FMath::Atan2(deltaY, deltaX) to get angle

					/// Decide whether to attack or walk (cannot do both)
					//if (PawnAsCharacter->CharacterAIShouldAttack())
					//{
					//	PawnAsCharacter->AddAttackInput();
					//}
					/////else
					//{
					//	// Move faster when facing toward the target so that we turn more accurately/don't orbit.
					//	PawnAsCharacter->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), FMath::GetMappedRangeValueClamped(FVector2D(-0.707f, 0.707f), FVector2D(0.0f, 1.0f), DotToTarget));
					//	// Attempt the entire turn in one frame. The Zombie itself will cap this, we're only expressing our desired turn amount here.
					//	PawnAsCharacter->AddRotationInput(DeltaYawDesired);
					//}
				}
			}
		}
	}
}

void AMyAIController::Possess(APawn* possessedPawn)
{
	Super::Possess(possessedPawn);

	//PawnAsCharacter = Cast<AMyCharacter>(possessedPawn);
}

void AMyAIController::UnPossess()
{


	Super::UnPossess();
	PawnAsCharacter = nullptr;
}

void AMyAIController::UpdateCachedPerception(APawn* fuckYou, float timePerecieved, ECharacterState characterState /* = ECharacterState::CannotTell */)
{
	if (fuckYou)
	{
		int32 num = CachedPerception.Num();
		for (int32 index = 0; index < num; ++index)
		{
			if (fuckYou == CachedPerception[index].Character)
			{
				CachedPerception[index] = FCachedPerception(fuckYou, characterState, timePerecieved);
				return;
			}
		}

		// If cannot find the character in the cached data, add it
		CachedPerception.Add(FCachedPerception(fuckYou, characterState, timePerecieved));
	}
}
