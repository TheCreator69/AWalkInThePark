

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SittingComponent.generated.h"

class AWalkPawn;
class AParkBench;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;
class UAnimMontage;

// Enum representing the status player is in with regards to sitting
UENUM(BlueprintType)
enum SitStatus
{
	ForcedSitting UMETA(DisplayName = "Forced To Sit"),
	Sitting UMETA(DisplayName = "Sitting"),
	Standing UMETA(DisplayName = "Standing"),
};

// Component used to manage changes in gameplay when the player sits down or stands back up
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API USittingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USittingComponent();

	// The mapping context used to define input actions available during walking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> WalkingMappingContext;

	// The mapping context used to define input actions available during sitting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> SittingMappingContext;

	// The montage played whent the player is sitting down
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SittingDownMontage;

	// The montage played whent the player is standing up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> StandingUpMontage;

	// Relative location of the gameplay camera while the player is sitting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector CameraRelativeSitLocation = FVector(12, 0, 48);

	// Relative location of the gameplay camera while the player is standing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector CameraRelativeStandLocation = FVector(12, 0, 64);

	// Time it takes to blend between montage and gameplay cameras
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraBlendTime = 0.25f;

private:
	AWalkPawn* Player;

	// Is the player currently sitting on a park bench?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<SitStatus> SitStatus = Standing;

	AParkBench* CurrentBench;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Change input and other gameplay elements when the player sits down
	void OnPlayerSitDown(AParkBench* TargetBench);

	// Allow the player to stand up again, if they are forced to sit right now
	void AllowPlayerToGetUp();

	// Change input and other gameplay elements when the player stands up
	void OnPlayerGetUp();

	// Immediately sit down without playing an animation and allow the player to get up immediately
	void SitDownForRespawn(AParkBench* TargetBench);

	// Whether the player is current sitting
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sitting")
	bool IsPlayerSitting() const;

	// Whether the player is allowed to stand up
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sitting")
	bool CanPlayerGetUp() const;

private:
	// Called after sitting down animation montage is complete
	UFUNCTION()
	void FinishSitDown(UAnimMontage* Montage, bool bInterrupted);

	// Called after standing up animation montage is complete
	UFUNCTION()
	void FinishGetUp(UAnimMontage* Montage, bool bInterrupted);

	void AddMappingContext(UInputMappingContext* MappingContext) const;

	void RemoveMappingContext(UInputMappingContext* MappingContext) const;

	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;

	void PlayAnimMontage(UAnimMontage* Montage) const;
};
