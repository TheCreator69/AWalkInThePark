

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/InteractiveActor.h"
#include "ParkBench.generated.h"

class UCameraComponent;
class AWalkPawn;
class AWalkPath;
class UArrowComponent;

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

	// Camera location when sitting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Component indicating the rotation the player starts at when initializing their "sitting down" animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> SitDownStartRotationComponent;

	// The walk path player should take after getting back up
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Sitting")
	TObjectPtr<AWalkPath> NextPath;

	// Relative location that the player initializes their "sitting down" animation from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sitting", meta = (MakeEditWidget))
	FVector SitDownStart = FVector(0);

	// Relative rotation that the player faces when initializing their "sitting down" animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sitting")
	FRotator SitDownStartRotation = FRotator(0);

	// A higher ID means the player encounters this park bench later. Make sure no two park benches in a level have the same ID.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Respawn")
	int ParkBenchID = 0;

private:
	// Whether the player can sit on this bench or not.
	// Will be false if the player has sat down on this bench or is currently sitting.
	bool bCanPlayerSit = true;

	TObjectPtr<AWalkPawn> Player;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Allow the player to get up again. Usually called after a scripted sitting event has finished
	UFUNCTION(BlueprintCallable, Category = "Sitting")
	void AllowPlayerToGetUp();

public:
	// Event dispatched when the player finishes sitting down (used for blueprint scripting)
	UFUNCTION(BlueprintImplementableEvent, Category = "Sitting", meta = (DisplayName = "PlayerSitDown"))
	void K2_OnPlayerSitDown();

	// Event dispatched when the player finishes standing up (used for blueprint scripting)
	UFUNCTION(BlueprintImplementableEvent, Category = "Sitting", meta = (DisplayName = "PlayerGetUp"))
	void K2_OnPlayerGetUp();

	// Used to sit the player down on the park bench
	void Interact_Implementation(AActor* Source);

	// C++ implementation of GetInteractionPrompt() from IInteractiveActor
	FText GetInteractionPrompt_Implementation();

	// C++ implementation of IsInteractive() from IInteractiveActor
	bool IsInteractive_Implementation();
	
	// Allow park bench to be sat on once again
	UFUNCTION(BlueprintCallable, Category = "Sitting")
	void ReactivateParkBench();

};
