

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SittingComponent.generated.h"

class AWalkPawn;
class AParkBench;
class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

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

private:
	AWalkPawn* Player;

	// Is the player currently sitting on a park bench?
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

	// Whether the player is current sitting
	bool IsPlayerSitting();

	// Whether the player is allowed to stand up
	bool CanPlayerGetUp() const;

private:
	void AddMappingContext(UInputMappingContext* MappingContext) const;

	void RemoveMappingContext(UInputMappingContext* MappingContext) const;

	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
