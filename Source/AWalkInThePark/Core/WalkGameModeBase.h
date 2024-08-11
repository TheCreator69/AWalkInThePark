

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WalkGameModeBase.generated.h"

class AWalkMonster;
class AShadowMonster;
class AParkBench;

// The current game's progress. Used for respawning logic
UENUM(BlueprintType)
enum EGameProgress
{
	Start UMETA(DisplayName = "Game started"),
	FirstParkBench UMETA(DisplayName = "First park bench reached"),
	SecondParkBench UMETA(DisplayName = "Second park bench reached"),
	ThirdParkBench UMETA(DisplayName = "Third park bench reached"),
};

/**
 * Default game mode used by the game. Default pawn and other classes are not set in the C++ implementation
 */
UCLASS()
class AWALKINTHEPARK_API AWalkGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Reload level to previous checkpoint and respawn the player in the correct state (i.e. standing/sitting)
	void ReloadLevelAndRespawnPlayer();

	// Progress the game further (or backwards, if you're crazy like that)
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void SetGameProgress(TEnumAsByte<EGameProgress> Progress);

	// First park bench player sits on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Respawning")
	TObjectPtr<AParkBench> FirstParkBench;

	// Second park bench player sits on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Respawning")
	TObjectPtr<AParkBench> SecondParkBench;

	// Third park bench player sits on
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Respawning")
	TObjectPtr<AParkBench> ThirdParkBench;

	// Water Monster Reference
	TObjectPtr<AWalkMonster> WaterMonster;

	// Park Monster Reference
	TObjectPtr<AWalkMonster> ParkMonster;

	// Shadow Monster Reference
	TObjectPtr<AShadowMonster> ShadowMonster;

private:
	TEnumAsByte<EGameProgress> CurrentProgress = Start;

	void ResetGameProgressToStart();
	void ResetGameProgressToFirstBench();
	void ResetGameProgressToSecondBench();
	void ResetGameProgressToThirdBench();
};
