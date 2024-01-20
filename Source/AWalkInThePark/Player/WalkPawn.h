

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WalkPawn.generated.h"

class UInputMappingContext;
class UInputAction;
class AWalkPath;
class USplineMovementComponent;
class UCameraComponent;
class UInteractionComponent;

// Default pawn implementing spline-based movement and other essential player functionality
UCLASS()
class AWALKINTHEPARK_API AWalkPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AWalkPawn();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Camera used as view target
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Component responsible for moving player along a spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineMovementComponent> SplineMovementComponent;

	// Component used to play music
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MusicPlayerComponent;

	// Component used to interact with interactive actors (what a mouthful!)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;
	
	// Music track used to mask the water monster's sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> MusicTrackWaterMask;

	// Music track used to mask the park monster's sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> MusicTrackParkMask;

	// The mapping context used to define input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;
	
	// Input action used for acceleration/deceleration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ChangeSpeedAction;

	// Input action used for looking around
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// Input action used toggling music
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleMusicAction;

	// Input action used for changing songs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ChangeSongAction;

	// Input action used for interaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	// Input action used for pausing/unpausing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TogglePauseAction;

private:
	// Is the current music being played the track that masks the water monster?
	bool bIsPlayingWaterMask = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Accelerate/decelerate the pawn along a spline
	UFUNCTION()
	void ChangeSpeed(const FInputActionValue& Value);

	// Look around with the camera
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// Toggle music player
	UFUNCTION()
	void ToggleMusic(const FInputActionValue& Value);

	// Change the song being played. Doesn't do anything if music is off
	UFUNCTION()
	void ChangeSong(const FInputActionValue& Value);

	// Interact with objects in the world (primarily used for sitting down/getting up)
	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	// Pause/Unpause the game
	UFUNCTION()
	void TogglePause(const FInputActionValue& Value);

};
