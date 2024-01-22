

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkMonster.generated.h"

USTRUCT(BlueprintType)
struct FAggressionChangeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	double BaseChange = 0.0;
	UPROPERTY(EditDefaultsOnly)
	double OffsetMin = 0.0;
	UPROPERTY(EditDefaultsOnly)
	double OffsetMax = 0.0;
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

	// The time between intervals where the monster's aggression changes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	double AggressionChangeRate = 1.0;

	// Values for increasing monster aggression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	FAggressionChangeInfo AggressionIncreaseInfo; //0.005, 0.0025, 0.004

	// Values for decreasing monster aggression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggression")
	FAggressionChangeInfo AggressionDecreaseInfo; //0.01, 0.007, 0.009

	/*
	* Curve that scale aggression changes based on how close the player's is to their max.
	* Used to increase aggression non-linearly as the player approaches max speed, penalizing it more.
	*/ 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aggression")
	TObjectPtr<UCurveFloat> AggressionChangeCurve;

private:
	// The monster's current aggression. Influences noise making
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double Aggression;

	// Handle used to change monster's aggression periodically
	FTimerHandle AggressionChangeTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Activate the monster so it can start stalking the player
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void ActivateMonster();

	// Deactivate the monster so it can become "passive" again. Also resets monster's aggression
	UFUNCTION(BlueprintCallable, Category = "Stalking")
	void DeactivateMonster();

	// Set the monster's aggression directly.
	UFUNCTION(BlueprintCallable, Category = "Aggression")
	void SetAggression(double NewAggression);

private:
	// Increase or decrease the monster's aggression on a timer
	void ChangeAggressionPeriodically();

	double CalculateAggressionChange(bool bLowerAggression, float SpeedPercentageMultiplier, double OffsetMin, double OffsetMax);

	// Whether or not the player is looking somewhat towards the monster and moving slowly enough to decrease its aggression
	bool IsPlayerStaringDownMonster();

};
