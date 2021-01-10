// Copyrighted Alastair Melville 2019

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridWorldRLActor.generated.h"


enum class EDirection
{
	Up,
	Down,
	Right,
	Left,

	Count,

	None
};

UENUM(BlueprintType)
enum class EPolicy : uint8
{
	None,

	Random,
	Fixed,
	Iterative,
	IterativeWindy,
	ValueIteration,
	MonteCarlo,
	MonteCarloRandom,
};

struct FGridEnvironment
{
	FGridEnvironment()
		: GridWidth(1)
		, GridHeight(1)
		, CurrentColumn(0)
		, CurrentRow(0)
	{}

	FGridEnvironment(int32 InGridWidth, int32 InGridHeight, int32 InStartColumn, int32 InStartRow)
		: GridWidth(InGridWidth)
		, GridHeight(InGridHeight)
		, CurrentColumn(InStartColumn)
		, CurrentRow(InStartRow)
	{}

	void Set(const TMap<FIntPoint /* State */, float>& InRewards, const TMap< FIntPoint /* State */, TArray<EDirection> >& InActions)
	{
		Rewards = InRewards;
		Actions = InActions;
	}

	void SetState(int32 InColumn, int32 InRow)
	{
		CurrentColumn = InColumn;
		CurrentRow = InRow;
	}

	void SetState(const FIntPoint& InState)
	{
		CurrentColumn = InState.X;
		CurrentRow = InState.Y;
	}

	FIntPoint GetCurrentState() const
	{
		return FIntPoint(CurrentColumn, CurrentRow);
	}

	// Returns whether we can move to any other state 
	bool IsTerminal(const FIntPoint& InAction) const
	{		
		const TArray<EDirection>* FoundValue = Actions.Find(InAction);
		return (FoundValue) ? false : true;
	}

	bool IsGameOver() const
	{
		const TArray<EDirection>* FoundValue = Actions.Find( FIntPoint(CurrentColumn, CurrentRow) );
		return (FoundValue) ? false : true;
	}

	TArray<FIntPoint> GetAllStates() const
	{
		TArray<FIntPoint> AllStates{};

		//TArray<FIntPoint> RewardKeys;
		Rewards.GetKeys(AllStates);

		//TArray<FIntPoint> ActionKeys;
		Actions.GetKeys(AllStates);

		return AllStates;
	}

	// Returns the reward for the move
	float Move(EDirection InAction);

	void UndoMove(EDirection InAction);

	static FGridEnvironment MakeStandardGrid();

	// Adds negative reward for moves - encourages the agent to solve the maze in the smallest number of moves.
	static FGridEnvironment MakeNegativeGrid(float StepCost = -0.1f);

	void PrintStateValues(const TMap<FIntPoint, float>& InStateValues);

	void PrintPolicy(const TMap<FIntPoint, EDirection>& InPolicy);

	int32 GridWidth;
	int32 GridHeight;
	int32 CurrentRow;
	int32 CurrentColumn;

	TMap<FIntPoint /* State */, float> Rewards;
	TMap< FIntPoint /* State */, TArray<EDirection> > Actions;
};

UCLASS()
class ANIMALATTRACTION_API AGridWorldRLActor : public AActor
{
	GENERATED_BODY()
	
public:
	AGridWorldRLActor();

	virtual void Tick(float DeltaTime) override;

	EDirection AddRandomnessToAction(EDirection InAction);

	TArray<struct FStateAndReward> PlayGame(FGridEnvironment InGridEnvironment, const TMap<FIntPoint, EDirection>& InPolicy);

protected:
	virtual void BeginPlay() override;

private:
	template<typename T>
	float GetMeanOfArray(const TArray<T>& InArray)
	{
		T Total = 0.0f;

		for (T Value : InArray)
		{
			Total += Value;
		}

		return Total / InArray.Num();
	}

	UPROPERTY(EditAnywhere)
	EPolicy Policy;
};
