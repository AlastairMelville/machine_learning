// Copyrighted Alastair Melville 2019

#include "ML/ReinforcementLearning/GridWorldRLActor.h"

#include "Kismet/KismetStringLibrary.h"


struct FStateAndReward
{
	FStateAndReward()
		: Reward(0.0f)
	{}

	FStateAndReward(const FIntPoint& InState, float InReward)
		: State(InState)
		, Reward(InReward)
	{}

	FIntPoint State;

	float Reward;
};

FString DirectionToString(EDirection InDirection)
{
	switch (InDirection)
	{
	case EDirection::Up:
	{
		return "U";
	}
	case EDirection::Down:
	{
		return "D";
	}
	case EDirection::Right:
	{
		return "R";
	}
	case EDirection::Left:
	{
		return "L";
	}
	default:
	{
		return "";
	}
	}
}

float FGridEnvironment::Move(EDirection InAction)
{
	const TArray<EDirection>* DirectionArray = Actions.Find(FIntPoint(CurrentColumn, CurrentRow));
	const bool bFound = (DirectionArray) ? DirectionArray->Find(InAction) != INDEX_NONE : false;
	if (!bFound)
	{
		return -2.0f;
	}

	switch (InAction)
	{
	case EDirection::Up:
	{
		CurrentRow -= 1;
		break;
	}
	case EDirection::Down:
	{
		CurrentRow += 1;
		break;
	}
	case EDirection::Right:
	{
		CurrentColumn += 1;
		break;
	}
	case EDirection::Left:
	{
		CurrentColumn -= 1;
		break;
	}
	default:
	{
		ensure(false);
		break;
	}
	}

	ensureAlways(GetAllStates().Find( FIntPoint(CurrentColumn, CurrentRow) ) != INDEX_NONE);

	float* Reward = Rewards.Find(FIntPoint(CurrentColumn, CurrentRow));
	return (Reward) ? *Reward : 0.0f;
}

void FGridEnvironment::UndoMove(EDirection InAction)
{
	switch (InAction)
	{
	case EDirection::Up:
	{
		CurrentRow += 1;
		break;
	}
	case EDirection::Down:
	{
		CurrentRow -= 1;
		break;
	}
	case EDirection::Right:
	{
		CurrentColumn -= 1;
		break;
	}
	case EDirection::Left:
	{
		CurrentColumn += 1;
		break;
	}
	default:
	{
		ensure(false);
		break;
	}
	}

	ensureAlways(GetAllStates().Find( FIntPoint(CurrentColumn, CurrentRow) ) != INDEX_NONE);
}

FGridEnvironment FGridEnvironment::MakeStandardGrid()
{
	FGridEnvironment Grid(4, 3, 2, 0);

	TMap<FIntPoint /* State */, float> Rewards{}; 
	Rewards.Add(FIntPoint(3, 0), 1.0f);
	Rewards.Add(FIntPoint(3, 1), -1.0f);

	TMap< FIntPoint /* State */, TArray<EDirection> > Actions{};
	Actions.Add(FIntPoint(0, 0), TArray<EDirection>{EDirection::Down, EDirection::Right});
	Actions.Add(FIntPoint(1, 0), TArray<EDirection>{EDirection::Left, EDirection::Right});
	Actions.Add(FIntPoint(2, 0), TArray<EDirection>{EDirection::Left, EDirection::Down, EDirection::Right});
	Actions.Add(FIntPoint(0, 1), TArray<EDirection>{EDirection::Up, EDirection::Down});
	Actions.Add(FIntPoint(2, 1), TArray<EDirection>{EDirection::Up, EDirection::Down, EDirection::Right});
	Actions.Add(FIntPoint(0, 2), TArray<EDirection>{EDirection::Up, EDirection::Right});
	Actions.Add(FIntPoint(1, 2), TArray<EDirection>{EDirection::Left, EDirection::Right});
	Actions.Add(FIntPoint(2, 2), TArray<EDirection>{EDirection::Left, EDirection::Right, EDirection::Up});
	Actions.Add(FIntPoint(3, 2), TArray<EDirection>{EDirection::Left, EDirection::Up});

	Grid.Set(Rewards, Actions);

	return Grid;
}

FGridEnvironment FGridEnvironment::MakeNegativeGrid(float StepCost /*= -0.1f*/)
{
	FGridEnvironment Grid = MakeStandardGrid();

	Grid.Rewards.Add(FIntPoint(0, 0), StepCost);
	Grid.Rewards.Add(FIntPoint(1, 0), StepCost);
	Grid.Rewards.Add(FIntPoint(2, 0), StepCost);
	Grid.Rewards.Add(FIntPoint(0, 1), StepCost);
	Grid.Rewards.Add(FIntPoint(2, 1), StepCost);
	Grid.Rewards.Add(FIntPoint(0, 2), StepCost);
	Grid.Rewards.Add(FIntPoint(1, 2), StepCost);
	Grid.Rewards.Add(FIntPoint(2, 2), StepCost);
	Grid.Rewards.Add(FIntPoint(3, 2), StepCost);

	return Grid;
}

void FGridEnvironment::PrintStateValues(const TMap<FIntPoint, float>& InStateValues)
{
	for (int32 GridRowIdx = 0; GridRowIdx < GridHeight; ++GridRowIdx)
	{
		FString GridRowStr;

		for (int32 GridColumnIdx = 0; GridColumnIdx < GridWidth; ++GridColumnIdx)
		{
			const float* Value = InStateValues.Find( FIntPoint(GridColumnIdx, GridRowIdx) );
			FString ValueAsStr = (Value) ? UKismetStringLibrary::Conv_FloatToString(*Value) : "   ";
			GridRowStr += ValueAsStr + " | ";
		}

		UE_LOG(LogScript, Warning, TEXT("%s"), *GridRowStr);
	}
}

void FGridEnvironment::PrintPolicy(const TMap<FIntPoint, EDirection>& InPolicy)
{
	for (int32 GridRowIdx = 0; GridRowIdx < GridHeight; ++GridRowIdx)
	{
		FString GridRowStr;

		for (int32 GridColumnIdx = 0; GridColumnIdx < GridWidth; ++GridColumnIdx)
		{
			const EDirection* Direction = InPolicy.Find(FIntPoint(GridColumnIdx, GridRowIdx));
			FString DirectionAsStr = (Direction) ? DirectionToString(*Direction) : " ";
			GridRowStr += DirectionAsStr + " | ";
		}

		UE_LOG(LogScript, Warning, TEXT("%s"), *GridRowStr);
	}
}

static const float SMALL_ENOUGH = 0.001f; // threshold for convergence

AGridWorldRLActor::AGridWorldRLActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGridWorldRLActor::BeginPlay()
{
	Super::BeginPlay();
	
	switch (Policy)
	{
	case EPolicy::Fixed:
	{
		FGridEnvironment Grid = FGridEnvironment::MakeStandardGrid();

		TArray<FIntPoint> States = Grid.GetAllStates();

		TMap<FIntPoint, float> StateValues{};

		TMap<FIntPoint, EDirection> FixedPolicy{};
		FixedPolicy.Add(FIntPoint(0, 2), EDirection::Up);
		FixedPolicy.Add(FIntPoint(0, 1), EDirection::Up);
		FixedPolicy.Add(FIntPoint(0, 0), EDirection::Right);
		FixedPolicy.Add(FIntPoint(1, 0), EDirection::Right);
		FixedPolicy.Add(FIntPoint(2, 0), EDirection::Right);
		FixedPolicy.Add(FIntPoint(2, 1), EDirection::Right);
		FixedPolicy.Add(FIntPoint(1, 2), EDirection::Right);
		FixedPolicy.Add(FIntPoint(2, 2), EDirection::Right);
		FixedPolicy.Add(FIntPoint(3, 2), EDirection::Up);

		const float Gamma = 0.9f;

		while (true)
		{
			float BiggestChange = 0.0f;

			for (const FIntPoint& State : States)
			{
				float& CurrentValue = StateValues.FindOrAdd(State);
				const float OldValue = CurrentValue;

				EDirection* Direction = FixedPolicy.Find(State);
				if (Direction)
				{
					Grid.SetState(State.X, State.Y);
					float Reward = Grid.Move(*Direction);
					const float& CurrentValNewState = StateValues.FindOrAdd(Grid.GetCurrentState());
					float NewValue = Reward + (Gamma * CurrentValNewState);
					BiggestChange = FMath::Max(BiggestChange, FMath::Abs(OldValue - NewValue));
				
					CurrentValue = NewValue;
				}
			}

			if (BiggestChange < SMALL_ENOUGH)
			{
				break;
			}
		}

		Grid.PrintStateValues(StateValues);

		break;
	}
	case EPolicy::Random:
	{
		FGridEnvironment Grid = FGridEnvironment::MakeStandardGrid();

		TArray<FIntPoint> States = Grid.GetAllStates();

		TMap<FIntPoint, float> StateValues{};

		const float Gamma = 1.0f;

		while (true)
		{
			float BiggestChange = 0.0f;

			for (const FIntPoint& State : States)
			{
				float& CurrentValue = StateValues.FindOrAdd(State);
				const float OldValue = CurrentValue;

				TArray<EDirection>* Directions = Grid.Actions.Find(State);
				if (Directions)
				{
					float NewValue = 0.0f;

					float UniRand = 1.0f / Directions->Num();
					for (EDirection Direction : *Directions)
					{
						Grid.SetState(State.X, State.Y);
						float Reward = Grid.Move(Direction);
						const float& CurrentValNewState = StateValues.FindOrAdd( Grid.GetCurrentState() );
						NewValue += UniRand * (Reward + Gamma * CurrentValNewState);
					}

					CurrentValue = NewValue;
					BiggestChange = FMath::Max(BiggestChange, FMath::Abs(OldValue - NewValue));
				}
			}

			if (BiggestChange < SMALL_ENOUGH)
			{
				break;
			}
		}

		Grid.PrintStateValues(StateValues);

		break;
	}
	case EPolicy::Iterative:
	{
		const float Gamma = 0.9f;

		FGridEnvironment NegativeGrid = FGridEnvironment::MakeNegativeGrid();

		NegativeGrid.PrintStateValues(NegativeGrid.Rewards);

		TArray<FIntPoint> NegativeGridStates = NegativeGrid.GetAllStates();

		TMap<FIntPoint, float> StateValues{};

		TArray<FIntPoint> ActionKeys;
		NegativeGrid.Actions.GetKeys(ActionKeys);

		TMap< FIntPoint /* State */, EDirection > Policy;
		for (const FIntPoint& Action : ActionKeys)
		{
			const int32 Rand = FMath::RandRange(0, (int32)EDirection::Count - 1);
			Policy.Add(Action, (EDirection)Rand);
		}

		NegativeGrid.PrintPolicy(Policy);

		for (const FIntPoint& State : NegativeGridStates)
		{
			const TArray<EDirection>* Directions = NegativeGrid.Actions.Find(State);
			float& CurrentValue = StateValues.FindOrAdd(State);
			if (Directions)
			{
				CurrentValue = FMath::RandRange(0.0f, 1.0f);
			}
			else
			{
				CurrentValue = 0.0f;
			}
		}

		while (true)
		{
			// Policy evaluation step
			while (true)
			{
				float BiggestChange = 0.0f;

				for (const FIntPoint& State : NegativeGridStates)
				{
					float& CurrentValue = StateValues.FindOrAdd(State);
					const float OldValue = CurrentValue;

					EDirection* Direction = Policy.Find(State);
					if (Direction)
					{
						NegativeGrid.SetState(State.X, State.Y);
						float Reward = NegativeGrid.Move(*Direction);
						const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
						float NewValue = Reward + (Gamma * CurrentValNewState);
						BiggestChange = FMath::Max(BiggestChange, FMath::Abs(OldValue - NewValue));

						CurrentValue = NewValue;
					}
				}

				if (BiggestChange < SMALL_ENOUGH)
				{
					break;
				}
			}

			// Policy improvement step
			bool IsPolicyConverged = true;
			for (const FIntPoint& State : NegativeGridStates)
			{
				EDirection* CurrentDirection = Policy.Find(State);
				if (CurrentDirection)
				{
					const EDirection OldDirection = *CurrentDirection;

					EDirection NewDirection = EDirection::None;
					float BestValue = FLT_MIN;
					// Loop through all possible actions to find the best current action.
					for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
					{
						EDirection DirectionI = (EDirection)DirectionIdx;
						NegativeGrid.SetState(State.X, State.Y);
						float Reward = NegativeGrid.Move(DirectionI);
						const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
						float NewValue = Reward + (Gamma * CurrentValNewState);

						if (NewValue > BestValue)
						{
							BestValue = NewValue;
							NewDirection = DirectionI;
						}
					}

					*CurrentDirection = NewDirection;
					if (OldDirection != NewDirection)
					{
						IsPolicyConverged = false;
					}
				}
			}

			if (IsPolicyConverged)
			{
				break;
			}
		}

		NegativeGrid.PrintStateValues(StateValues);
		NegativeGrid.PrintPolicy(Policy);

		break;
	}
	case EPolicy::IterativeWindy:
	{
		const float Gamma = 0.9f;

		FGridEnvironment NegativeGrid = FGridEnvironment::MakeNegativeGrid(-1.0f);

		NegativeGrid.PrintStateValues(NegativeGrid.Rewards);

		TArray<FIntPoint> NegativeGridStates = NegativeGrid.GetAllStates();

		TMap<FIntPoint, float> StateValues{};

		TArray<FIntPoint> ActionKeys;
		NegativeGrid.Actions.GetKeys(ActionKeys);

		TMap< FIntPoint /* State */, EDirection > Policy;
		for (const FIntPoint& Action : ActionKeys)
		{
			const int32 Rand = FMath::RandRange(0, (int32)EDirection::Count - 1);
			Policy.Add(Action, (EDirection)Rand);
		}

		NegativeGrid.PrintPolicy(Policy);

		for (const FIntPoint& State : NegativeGridStates)
		{
			const TArray<EDirection>* Directions = NegativeGrid.Actions.Find(State);
			float& CurrentValue = StateValues.FindOrAdd(State);
			if (Directions)
			{
				CurrentValue = FMath::RandRange(0.0f, 1.0f);
			}
			else
			{
				CurrentValue = 0.0f;
			}
		}

		while (true)
		{
			// Policy evaluation step
			while (true)
			{
				float BiggestChange = 0.0f;

				for (const FIntPoint& State : NegativeGridStates)
				{
					float& CurrentValue = StateValues.FindOrAdd(State);
					const float OldValue = CurrentValue;

					float NewValue = 0.0f;

					EDirection* Direction = Policy.Find(State);
					if (Direction)
					{
						for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
						{
							float Probability = 0.5f;

							EDirection DirectionI = (EDirection)DirectionIdx;
							if (DirectionI != *Direction)
							{
								Probability /= 3.0f;
							}

							NegativeGrid.SetState(State.X, State.Y);
							float Reward = NegativeGrid.Move(DirectionI);
							const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
							NewValue += Probability * (Reward + (Gamma * CurrentValNewState));
						}

						BiggestChange = FMath::Max(BiggestChange, FMath::Abs(OldValue - NewValue));

						CurrentValue = NewValue;
					}
				}

				if (BiggestChange < SMALL_ENOUGH)
				{
					break;
				}
			}

			// Policy improvement step
			bool IsPolicyConverged = true;
			for (const FIntPoint& State : NegativeGridStates)
			{
				EDirection* CurrentDirection = Policy.Find(State);
				if (CurrentDirection)
				{
					const EDirection OldDirection = *CurrentDirection;

					EDirection NewDirection = EDirection::None;
					float BestValue = -50.0f;
					// Loop through all possible actions to find the best current action.
					for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
					{
						EDirection DirectionI = (EDirection)DirectionIdx;
						float NewValue = 0.0f;

						for (int32 DirectionIdxJ = 0; DirectionIdxJ < (int32)EDirection::Count; ++DirectionIdxJ)
						{
							EDirection DirectionJ = (EDirection)DirectionIdxJ;

							float Probability = 0.5f;
							if (DirectionJ != DirectionI)
							{
								Probability /= 3.0f;
							}

							NegativeGrid.SetState(State.X, State.Y);
							float Reward = NegativeGrid.Move(DirectionJ);
							const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
							NewValue += Probability * (Reward + (Gamma * CurrentValNewState));
						}

						if (NewValue > BestValue)
						{
							BestValue = NewValue;
							NewDirection = DirectionI;
						}
					}

					*CurrentDirection = NewDirection;
					if (OldDirection != NewDirection)
					{
						IsPolicyConverged = false;
					}
				}
			}

			if (IsPolicyConverged)
			{
				break;
			}
		}

		NegativeGrid.PrintStateValues(StateValues);
		NegativeGrid.PrintPolicy(Policy);

		break;
	}
	case EPolicy::ValueIteration:
	{
		const float Gamma = 0.9f;

		FGridEnvironment NegativeGrid = FGridEnvironment::MakeNegativeGrid();

		NegativeGrid.PrintStateValues(NegativeGrid.Rewards);

		TArray<FIntPoint> NegativeGridStates = NegativeGrid.GetAllStates();

		TMap<FIntPoint, float> StateValues{};

		TArray<FIntPoint> ActionKeys;
		NegativeGrid.Actions.GetKeys(ActionKeys);

		TMap< FIntPoint /* State */, EDirection > Policy;
		for (const FIntPoint& Action : ActionKeys)
		{
			const int32 Rand = FMath::RandRange(0, (int32)EDirection::Count - 1);
			Policy.Add(Action, (EDirection)Rand);
		}

		NegativeGrid.PrintPolicy(Policy);

		for (const FIntPoint& State : NegativeGridStates)
		{
			const TArray<EDirection>* Directions = NegativeGrid.Actions.Find(State);
			float& CurrentValue = StateValues.FindOrAdd(State);
			if (Directions)
			{
				CurrentValue = FMath::RandRange(0.0f, 1.0f);
			}
			else
			{
				CurrentValue = 0.0f;
			}
		}

		// Policy evaluation step
		while (true)
		{
			float BiggestChange = 0.0f;

			for (const FIntPoint& State : NegativeGridStates)
			{
				float& CurrentValue = StateValues.FindOrAdd(State);
				const float OldValue = CurrentValue;

				EDirection* Direction = Policy.Find(State);
				if (Direction)
				{
					float NewValue = -100.0f;
					// Loop through all possible actions to find the best current action.
					for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
					{
						EDirection DirectionI = (EDirection)DirectionIdx;

						NegativeGrid.SetState(State.X, State.Y);
						float Reward = NegativeGrid.Move(DirectionI);
						const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
						float Value = Reward + (Gamma * CurrentValNewState);

						// Check if this is the best action so far.
						if (Value > NewValue)
						{
							NewValue = Value;
						}
					}

					CurrentValue = NewValue;

					BiggestChange = FMath::Max(BiggestChange, FMath::Abs(OldValue - NewValue));
				}
			}

			if (BiggestChange < SMALL_ENOUGH)
			{
				break;
			}
		}

		// Policy improvement step
		TArray<FIntPoint> PolicyKeys;
		Policy.GetKeys(PolicyKeys);

		for (const FIntPoint& State : PolicyKeys)
		{
			EDirection BestDirection = EDirection::None;

			float BestValue = -100.0f;
			// Loop through all possible actions to find the best current action.
			for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
			{
				EDirection DirectionI = (EDirection)DirectionIdx;

				NegativeGrid.SetState(State.X, State.Y);
				float Reward = NegativeGrid.Move(DirectionI);
				const float& CurrentValNewState = StateValues.FindOrAdd(NegativeGrid.GetCurrentState());
				float NewValue = Reward + (Gamma * CurrentValNewState);

				if (NewValue > BestValue)
				{
					BestValue = NewValue;
					BestDirection = DirectionI;
				}
			}

			EDirection* CurrentDirection = Policy.Find(State);
			*CurrentDirection = BestDirection;
		}
		
		NegativeGrid.PrintStateValues(StateValues);
		NegativeGrid.PrintPolicy(Policy);

		break;
	}
	case EPolicy::MonteCarlo:
	{
		FGridEnvironment StandardGrid = FGridEnvironment::MakeStandardGrid();

		StandardGrid.PrintStateValues(StandardGrid.Rewards);

		TMap<FIntPoint, EDirection> Policy{};
		Policy.Add(FIntPoint(0, 2), EDirection::Up);
		Policy.Add(FIntPoint(0, 1), EDirection::Up);
		Policy.Add(FIntPoint(0, 0), EDirection::Right);
		Policy.Add(FIntPoint(1, 0), EDirection::Right);
		Policy.Add(FIntPoint(2, 0), EDirection::Right);
		Policy.Add(FIntPoint(2, 1), EDirection::Right);
		Policy.Add(FIntPoint(1, 2), EDirection::Right);
		Policy.Add(FIntPoint(2, 2), EDirection::Right);
		Policy.Add(FIntPoint(3, 2), EDirection::Up);

		StandardGrid.PrintPolicy(Policy);

		TArray<FIntPoint> StandardGridStates = StandardGrid.GetAllStates();
		TMap<FIntPoint, float> StateValues{};
		TMap< FIntPoint, TArray<float> > Returns{};

		for (const FIntPoint& State : StandardGridStates)
		{
			const TArray<EDirection>* Directions = StandardGrid.Actions.Find(State);
			float& CurrentValue = StateValues.FindOrAdd(State);
			if (Directions)
			{
				Returns.FindOrAdd(State);
			}
			else
			{
				CurrentValue = 0.0f;
			}
		}

		for (int32 InterationIdx = 0; InterationIdx < 100; ++InterationIdx)
		{
			TArray<FStateAndReward> StatesAndRewards = PlayGame(StandardGrid, Policy);
			TSet<FIntPoint> SeenStates;

			for (const FStateAndReward& StateAndReward : StatesAndRewards)
			{
				FIntPoint* FoundReward = SeenStates.Find(StateAndReward.State);
				if (!FoundReward)
				{
					TArray<float>& Rewards = Returns.FindOrAdd(StateAndReward.State);
					Rewards.Add(StateAndReward.Reward);
					
					float& Value = StateValues.FindOrAdd(StateAndReward.State);
					Value = GetMeanOfArray<float>(Rewards);

					SeenStates.Add(StateAndReward.State);
				}
			}
		}

		StandardGrid.PrintStateValues(StateValues);
		StandardGrid.PrintPolicy(Policy);

		break;
	}
	case EPolicy::MonteCarloRandom:
	{
		FGridEnvironment StandardGrid = FGridEnvironment::MakeStandardGrid();

		StandardGrid.PrintStateValues(StandardGrid.Rewards);

		TMap<FIntPoint, EDirection> Policy{};
		Policy.Add(FIntPoint(0, 2), EDirection::Up);
		Policy.Add(FIntPoint(0, 1), EDirection::Up);
		Policy.Add(FIntPoint(0, 0), EDirection::Right);
		Policy.Add(FIntPoint(1, 0), EDirection::Right);
		Policy.Add(FIntPoint(2, 0), EDirection::Right);
		Policy.Add(FIntPoint(2, 1), EDirection::Up);
		Policy.Add(FIntPoint(1, 2), EDirection::Left);
		Policy.Add(FIntPoint(2, 2), EDirection::Up);
		Policy.Add(FIntPoint(3, 2), EDirection::Left);

		StandardGrid.PrintPolicy(Policy);

		TArray<FIntPoint> StandardGridStates = StandardGrid.GetAllStates();
		TMap<FIntPoint, float> StateValues{};
		TMap< FIntPoint, TArray<float> > Returns{};

		for (const FIntPoint& State : StandardGridStates)
		{
			const TArray<EDirection>* Directions = StandardGrid.Actions.Find(State);
			float& CurrentValue = StateValues.FindOrAdd(State);
			if (Directions)
			{
				Returns.FindOrAdd(State);
			}
			else
			{
				CurrentValue = 0.0f;
			}
		}

		for (int32 InterationIdx = 0; InterationIdx < 5000; ++InterationIdx)
		{
			TArray<FStateAndReward> StatesAndRewards = PlayGame(StandardGrid, Policy);
			TSet<FIntPoint> SeenStates;

			for (const FStateAndReward& StateAndReward : StatesAndRewards)
			{
				FIntPoint* FoundReward = SeenStates.Find(StateAndReward.State);
				if (!FoundReward)
				{
					TArray<float>& Rewards = Returns.FindOrAdd(StateAndReward.State);
					Rewards.Add(StateAndReward.Reward);

					float& Value = StateValues.FindOrAdd(StateAndReward.State);
					Value = GetMeanOfArray<float>(Rewards);

					SeenStates.Add(StateAndReward.State);
				}
			}
		}

		StandardGrid.PrintStateValues(StateValues);
		StandardGrid.PrintPolicy(Policy);

		break;
	}
	default:
	{
		break;
	}
	}
}

void AGridWorldRLActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EDirection AGridWorldRLActor::AddRandomnessToAction(EDirection InAction)
{
	const float Rand = FMath::RandRange(0.0f, 1.0f);
	if (Rand < 0.5f)
	{
		return InAction;
	}
	else
	{
		TArray<EDirection> AllDirections;
		for (int32 DirectionIdx = 0; DirectionIdx < (int32)EDirection::Count; ++DirectionIdx)
		{
			EDirection Direction = (EDirection)DirectionIdx;

			// Exclude InAction
			if (Direction != InAction)
			{
				AllDirections.Add(Direction);
			}
		}

		const int32 RandIndex = FMath::RandRange(0, AllDirections.Num() - 1);
		return AllDirections[RandIndex];
	}
}

TArray<FStateAndReward> AGridWorldRLActor::PlayGame(FGridEnvironment InGridEnvironment, const TMap<FIntPoint, EDirection>& InPolicy)
{
	static float Gamma = 0.9f;

	TArray<FIntPoint> StartStatesOptions;
	InGridEnvironment.Actions.GetKeys(StartStatesOptions);
	const int32 RandIndex = FMath::RandRange(0, StartStatesOptions.Num() - 1);
	InGridEnvironment.SetState(StartStatesOptions[RandIndex]);

	FIntPoint CurrentState = InGridEnvironment.GetCurrentState();

	TArray<FStateAndReward> StateAndRewards{ FStateAndReward(CurrentState, 0.0f) };

	while (!InGridEnvironment.IsGameOver())
	{
		const EDirection* Direction = InPolicy.Find(CurrentState);
		if (Direction)
		{
			EDirection Action = *Direction;
			Action = AddRandomnessToAction(*Direction);
			float Reward = InGridEnvironment.Move(Action);
			CurrentState = InGridEnvironment.GetCurrentState();
			StateAndRewards.Add( FStateAndReward(CurrentState, Reward) );
		}
	}

	// Calculate the returns by working backwards from the terminal state
	float G = 0.0f;
	TArray<FStateAndReward> StateAndReturns{};
	
	G = StateAndRewards.Last().Reward + (Gamma * G);

	for (int32 Index = StateAndRewards.Num() - 2 /*ignore last element*/; Index >= 0; --Index)
	{
		const FStateAndReward& State = StateAndRewards[Index];

		StateAndReturns.Insert( FStateAndReward(State.State, G), 0);
		G = State.Reward + (Gamma * G);
	}

	return StateAndReturns;
}
