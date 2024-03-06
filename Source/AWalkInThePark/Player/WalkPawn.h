

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
class USanityComponent;
class UCapsuleComponent;
class AParkBench;
class USittingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMusicPlayStateChanged, bool, bIsPaused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartOverlapSafeZone);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndOverlapSafeZone);

UENUM(BlueprintType)
enum PlayerDeathReason
{
	Insanity UMETA(DisplayName = "Insanity"),
	Monster UMETA(DisplayName = "Monster"),
};

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

	// Camera used as view target during animation montages (used for camera movement bound to animations)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> MontageCameraComponent;

	// Capsule component used for collision detection with triggers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	// Skeletal mesh component used for visual representation in the world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	// Component responsible for moving player along a spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineMovementComponent> SplineMovementComponent;

	// Component used to play music
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> MusicPlayerComponent;

	// Component used to play low sanity ambience
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> LowSanityAmbienceComponent;

	// Component used to play intrusive thoughts
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> IntrusiveThoughtsComponent;

	// Component used to interact with interactive actors (what a mouthful!)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractionComponent> InteractionComponent;

	// Component used to manage sanity and sanity-related effects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USanityComponent> SanityComponent;

	// Component used to manage gameplay changes when sitting down and standing up
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USittingComponent> SittingComponent;
	
	// Music track used to mask the water monster's sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> MusicTrackWaterMask;

	// Music track used to mask the park monster's sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TObjectPtr<USoundBase> MusicTrackParkMask;

	// The mapping context used to define input actions that should always be available
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> GlobalMappingContext;
	
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

	// Input action used for standing up from a park bench
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> GetUpAction;

	// Input action used for saving the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SaveGameAction;

	// Event dispatched when the music starts/stops playing
	UPROPERTY(BlueprintAssignable, Category = "Music")
	FMusicPlayStateChanged OnMusicStateChanged;

	// Event dispatched when player starts overlapping a safe zone
	UPROPERTY(BlueprintAssignable, Category = "Safe Zone")
	FStartOverlapSafeZone OnBeginOverlapSafeZone;

	// Event dispatched when player stops overlapping a safe zone
	UPROPERTY(BlueprintAssignable, Category = "Safe Zone")
	FEndOverlapSafeZone OnEndOverlapSafeZone;

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

	// Kill the player!!! And also specify a reason
	UFUNCTION()
	void KillPlayer(TEnumAsByte<PlayerDeathReason> Reason);

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

	// Get up from a park bench
	UFUNCTION()
	void GetUp(const FInputActionValue& Value);

	// Save the game
	UFUNCTION()
	void SaveGame(const FInputActionValue& Value);

};
