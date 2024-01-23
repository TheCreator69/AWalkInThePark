

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/InteractiveActor.h"
#include "ParkBench.generated.h"

class UCameraComponent;
class AWalkPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerSatDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerGotUp);

// The park benches the player uses to rest and save their progress on. Also serve as instigators for new monster introductions
UCLASS()
class AWALKINTHEPARK_API AParkBench : public AActor, public IInteractiveActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AParkBench();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Bench static mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BenchComponent;

	// View target when sitting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Event dispatched when the player sits down
	UPROPERTY(BlueprintAssignable, Category = "Sitting")
	FPlayerSatDown OnPlayerSitDown;

	// Event dispatched when the player stands up
	UPROPERTY(BlueprintAssignable, Category = "Sitting")
	FPlayerGotUp OnPlayerStandUp;

	// How long the player is forced to sit down. Remove once sit event system is properly implemented.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sitting")
	double ForcedSitTime = 0.0;

private:
	// Whether the player can sit on this bench or not
	bool bCanPlayerSit = false;

	// Reference to player
	TObjectPtr<AWalkPawn> Player;

	// Timer handle to end forced sitting
	FTimerHandle ForcedSitEndTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// C++ implementation of Interact() from IInteractiveActor
	void Interact_Implementation(AActor* Source);

private:
	// Allow player to get up again
	void AllowPlayerToStand();

public:
	// Function called by Player pawn when it gets up
	void GetUp();

	// C++ implementation of GetInteractionPrompt() from IInteractiveActor
	FText GetInteractionPrompt_Implementation();

	// C++ implementation of IsInteractive() from IInteractiveActor
	bool IsInteractive_Implementation();

};
