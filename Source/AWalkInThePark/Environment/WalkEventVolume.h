

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkEventVolume.generated.h"

class UBoxComponent;
struct FHitResult;
class AWalkPawn;

// Base class for all volumes/triggers which cause in-game events to occur when the player overlaps or stops overlapping with them
UCLASS()
class AWALKINTHEPARK_API AWalkEventVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkEventVolume();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Volume used to detect player overlaps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> VolumeComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Check if the player overlapped and only then fire the "actual" overlap function
	UFUNCTION()
	void BeginOverlapCheckIfPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Check if the player stopped overlapping and only then fire the "actual" overlap function
	UFUNCTION()
	void EndOverlapCheckIfPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// C++ implementation for overlap events. Could be handled by BlueprintNativeEvent, but this causes chaos when trying to inherit from this class.

	// Player has started overlapping the gameplay volume (C++ implementation)
	virtual void PlayerBeginOverlapInternal(AWalkPawn* Player);

	// Player has stopped overlapping the gameplay volume (C++ implementation)
	virtual void PlayerEndOverlapInternal(AWalkPawn* Player);

public:
	// Player has started overlapping the gameplay volume
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Event")
	void PlayerBeginOverlap(AWalkPawn* Player);

	// Player has stopped overlapping the gameplay volume
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Event")
	void PlayerEndOverlap(AWalkPawn* Player);

};
