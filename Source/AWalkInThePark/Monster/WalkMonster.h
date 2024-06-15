

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkMonster.generated.h"

class UPlayerFollowComponent;
class UMonsterFootstepAudioComponent;

USTRUCT(BlueprintType)
struct FAggressionChangeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double BaseChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double OffsetMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double OffsetMax;

	FAggressionChangeInfo()
	{
		BaseChange = 0.0;
		OffsetMin = 0.0;
		OffsetMax = 0.0;
	}

	FAggressionChangeInfo(double Base, double Min, double Max)
	{
		BaseChange = Base;
		OffsetMin = Min;
		OffsetMax = Max;
	}
};

// Base class for the park and water monsters that stalk the player throughout the game.
UCLASS()
class AWALKINTHEPARK_API AWalkMonster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkMonster();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Debug cube used to display the monster's location in-game
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DebugCube;

	// Component used to snap to player pawn's location with offset but without inheriting its rotation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerFollowComponent> PlayerFollowComponent;

	// Component used to schedule and play monster footstep audio under certain conditions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMonsterFootstepAudioComponent> FootstepAudioComponent;

	// The time between intervals where the monster's aggression changes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	double AggressionChangeRate = 1.0;

	// Values for increasing monster aggression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	FAggressionChangeInfo AggressionIncreaseInfo = FAggressionChangeInfo(0.005, 0.0025, 0.004);

	// Values for decreasing monster aggression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	FAggressionChangeInfo AggressionDecreaseInfo = FAggressionChangeInfo(0.01, 0.007, 0.009);

	/*
	* Curve that scale aggression changes based on how close the player's is to their max.
	* Used to increase aggression non-linearly as the player approaches max speed, penalizing it more.
	*/ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aggression")
	TObjectPtr<UCurveFloat> AggressionChangeCurve;

	/* Is this monster the water monster ? If not, assume this monster is the park monster.
	Used for player death to know which monster they died from.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	bool bIsWaterMonster = true;

private:
	// The monster's current aggression. Influences noise making
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double Aggression;

	// Handle used to change monster's aggression periodically
	FTimerHandle AggressionChangeTimerHandle;

	// Can this monster be activated by ReactivateMonster()?
	bool bCanBeReactivated = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Activate the monster so it can start stalking the player. Used by gameplay events to first activate the monster
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void ActivateMonster();

	// Deactivate the monster so it can become "passive" again. Also resets monster's aggression
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void DeactivateMonster();

	// Reactivate the monster so it can start stalking the player. Used by safe zone to reactivate monster, is possible
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void ReactivateMonster();

	// Prevent the monster from being reactivated. Doesn't deactivate the monster
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void DisableReactivation();

public:
	// Get the monster's current aggression
	UFUNCTION(BlueprintCallable, Category = "Aggression")
	double GetAggression() const;

	// Set the monster's aggression directly.
	UFUNCTION(BlueprintCallable, Category = "Aggression")
	void SetAggression(double NewAggression);

private:
	// Increase or decrease the monster's aggression on a timer
	void ChangeAggressionPeriodically();

	double CalculateAggressionChange(bool bLowerAggression, float SpeedPercentageMultiplier, double OffsetMin, double OffsetMax);

public:
	// Whether or not the player is looking somewhat towards the monster and moving slowly enough to decrease its aggression
	bool IsPlayerStaringDownMonster();

};
