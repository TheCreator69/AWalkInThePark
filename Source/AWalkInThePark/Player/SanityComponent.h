

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SanityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSanityReachedZero);

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Override the current sanity with a new value
	void SetSanity(float NewSanity);

	// Activate/deactivate decreasing sanity. If deactivated, sanity will increase instead
	UFUNCTION()
	void SetDecreaseSanity(bool bNewDecreaseSanity);

	// Event dispatched when sanity reaches zero
	UPROPERTY(BlueprintAssignable, Category = "Sanity")
	FSanityReachedZero OnSanityReachedZero;

private:
	float Sanity = 1.f;

	// Should this component decrease the sanity at a regular interval?
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bDecreaseSanity = false;

	// Increase/decrease sanity. Used in tick function
	void UpdateSanity(float DeltaTime);

	// Update sanity-related effects
	void UpdateSanityEffects();

	// Get a alpha value based on the threshold put in. Value is at 0 if Sanity = Threshold and at 1 if Sanity = 0
	float InvertedSanityAlpha(float Threshold) const;

	// Lerp between two FVector4 for low sanity post process effects
	FVector4 PostProcessLerp(double A, double B, float Alpha) const;

	// Timer handle used to play looping intrusive thoughts at low sanity.
	FTimerHandle IntrusiveThoughtTimerHandle;

	void PlayIntrusiveThought();
};
