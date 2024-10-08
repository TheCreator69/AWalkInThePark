

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "MonsterFootstepAudioComponent.generated.h"

class AWalkMonster;

// Enum used to return info about whether a threshold was crossed and in which direction
UENUM(BlueprintType)
enum EThresholdCrossing
{
	Upwards UMETA(DisplayName = "Upwards"),
	Downwards UMETA(DisplayName = "Downwards"),
	NoCross UMETA(DisplayName = "No Cross"),
};

// Aggression thresholds that cause a change in sounds when crossed
USTRUCT(BlueprintType)
struct FSoundThresholds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double StartFootstepSounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double StopFootstepSounds;

	FSoundThresholds()
	{
		StartFootstepSounds = 0.5;
		StopFootstepSounds = 0.25;
	}
};

// Component used to play monster footstep sounds. Controls which sounds to play and schedules its own sounds.
UCLASS()
class AWALKINTHEPARK_API UMonsterFootstepAudioComponent : public UAudioComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UMonsterFootstepAudioComponent();

	// Footstep sound the monster makes when it's retreating
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	TObjectPtr<USoundBase> RetreatingFootsteps;

	// Footstep sound the monster makes when it's at high aggression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
	TObjectPtr<USoundBase> ApproachingFootsteps;

	// Thresholds used to determine at which aggression certain actions should be performed (sound changes, starts & stops)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Threshold")
	FSoundThresholds SoundThresholds;

	// Minimum interval at which footstep play. Footsteps get more frequent the higher the monster's aggression is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps|Interval", meta = (UIMin = 0, UIMax = 10))
	float IntervalMin = 0.5f;

	// Maximum interval at which footstep play. Footsteps get more frequent the higher the monster's aggression is
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps|Interval", meta = (UIMin = 0, UIMax = 10))
	float IntervalMax = 5.0f;

	// Random offset to add some variation to footstep frequency. Can both increase and decrease interval
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps|Interval", meta = (UIMin = 0, UIMax = 1))
	float RandomIntervalOffset = 0.4f;

private:
	TObjectPtr<AWalkMonster> Owner;

	// Handle for the timer that plays footstep sounds
	FTimerHandle FootstepTimerHandle;

	// Value of monster aggression the previous tick. Used to compare whether aggression has crossed a threshold this tick.
	double PreviousAggression;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ExecuteAggressionThresholdFunctions(double& OldAggression, double& NewAggression);

	/*
	* Schedule next footstep sound and keep playing a new footstep sound after the current one is finished.
	* Once this function is called, it will bind to the "audio finished" delegate to call itself again after the current sound stops playing, essentially creating a loop.
	* To stop footstep sounds completely, call StopMakingSound()
	*/
	void StartMakingSound();

	// Schedule the next footstep sound with a random delay
	UFUNCTION()
	void ScheduleNextSound();

	// Play the appropriate footstep sound at the proper volume relative to monster's current aggression
	void PlayRightSoundAtProperVolume();

	// Cancel any scheduled footstep sound and prevent new sound from being scheduled if a sound is still playing
	void StopMakingSound();

	/*
	* Returns whether the threshold was crossed between OldAggression and NewAggression and in which direction (upwards or downwards)
	* If OldAggression and NewAggression are the same, the threshold isn't considered crossed, even if Threshold is identical to aggressions passed in
	*/
	TEnumAsByte<EThresholdCrossing> AggressionCrossedThreshold(double& OldAggression, double& NewAggression, double Threshold);
};
