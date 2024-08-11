

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "SanityComponent.generated.h"

// Info struct for a transition between two thought states, to be used as data for thought states.
USTRUCT(BlueprintType)
struct FThoughtTransition
{
	GENERATED_BODY()

public:
	// Row name of the thought to transition to in a data table.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetRowName;

	// Weight of the chance to transition to this thought. Will be weighed against all other transition weights for thought state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChanceWeight;

	FThoughtTransition()
	{
		TargetRowName = FName();
		ChanceWeight = 1.f;
	}
};

// Info struct for a single intrusive thought state, to be used as basis for a data table asset.
USTRUCT(BlueprintType)
struct FThoughtState : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Thought sound wave.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> Sound;

	// Thought states to transition to, along with their chances of transitioning.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FThoughtTransition> Transitions;

	// Whether this state can be used as the start state when entering the state machine.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeStartState;

	// Prevents a warning or error when packaging the project according to this snippet:
	// https://dev.epicgames.com/community/snippets/7LX/udatatable-quick-start
	FThoughtState() : Sound(nullptr), Transitions(TArray<FThoughtTransition>()), bCanBeStartState(true)
	{
	}
};

// Component managing player's current sanity (capped between 0 and 1 no cap) and the effects that come with low sanity
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API USanityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USanityComponent();

	// How fast the sanity should decrease per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float SanityDecreaseRate = 0.01f;

	// How fast the sanity should increase per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float SanityIncreaseRate = 0.02f;

	// At which sanity percentage the low sanity ambience should start to fade in
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity Effects", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float AmbienceThreshold = 0.6f;

	// At which sanity percentage the camera tilt should start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity Effects", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float CameraTiltThreshold = 0.4f;

	// At which sanity percentage post process effects should start being applied to the player's camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity Effects", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float PostProcessThreshold = 0.2f;

	// At which sanity percentage intrusive thoughts should pop up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sanity Effects", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 1.f, ClampMax = 1.f))
	float IntrusiveThoughtsThreshold = 0.2f;

	// Data table asset creating a state machine for intrusive thoughts. Used to chain related thoughts together and to enable "looping thought trains".
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sanity Effects", meta=(RowType="FThoughtState"))
	TObjectPtr<UDataTable> ThoughtStateMachine;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Override the current sanity with a new value
	void SetSanity(float NewSanity);

	// Activate/deactivate decreasing sanity. If deactivated, sanity will increase instead
	UFUNCTION(BlueprintCallable, Category = "Sanity")
	void SetDecreaseSanity(bool bNewDecreaseSanity);

	// Set sanity right above the threshold where sanity effects kick in
	UFUNCTION(BlueprintCallable)
	void TriggerEffectsImmediately();

	// Allow sanity to decrease
	UFUNCTION(BlueprintCallable)
	void AllowDecreasingSanity();

	// Prohibit sanity from decreasing and restore sanity
	UFUNCTION(BlueprintCallable)
	void ProhibitDecreasingSanityAndReset();

private:
	float Sanity = 1.f;

	// Should sanity currently be decreased? Has no effect is bCanDecreaseSanity is set to false
	bool bDecreaseSanity = true;

	// Can sanity currently be decreased? False if player is in safe zone, for instance.
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bCanDecreaseSanity = false;

	// Increase/decrease sanity. Used in tick function
	void UpdateSanity(float DeltaTime);

	// Update sanity-related effects
	void UpdateSanityEffects();

	// Get a alpha value based on the threshold put in. Value is at 0 if Sanity = Threshold and at 1 if Sanity = 0
	float InvertedSanityAlpha(float Threshold) const;

	// Lerp between two FVector4 for low sanity post process effects
	FVector4 PostProcessLerp(double A, double B, float Alpha) const;

	// The thought state that is currently selected. Null when intrusive thoughts aren't playing.
	FThoughtState* CurrentThoughtState;

	// Select a random valid intrusive thought to start from, play it and schedule the next one after the current one has finished playing.
	void StartIntrusiveThoughts();

	// Stop new intrusive thoughts from playing.
	void StopIntrusiveThoughts();

	// Select a new intrusive thought to play and schedule the next selection.
	UFUNCTION()
	void SelectAndPlayNewThought();

	// Play the currently selected intrusive thought with the appropriate volume in regards to player's sanity.
	void PlayIntrusiveThought();
};
