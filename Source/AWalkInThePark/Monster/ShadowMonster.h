

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShadowMonster.generated.h"

class USplineMovementComponent;
class AWalkPath;
class USphereComponent;
class UCameraComponent;

// Class for the shadow monster that chases the player towards the end of the game
UCLASS()
class AWALKINTHEPARK_API AShadowMonster : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShadowMonster();

	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// Collision sphere that disables any overlapping lights
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> LightDisabler;

	// Camera component used as view target
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Mesh used to display shadow in-game
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShadowMesh;

	// Component used to snap monster to a spline, just like the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineMovementComponent> SplineMovementComponent;

private:
	// Amount to accelerate during each tick
	float AccelerationPerTick = 0.f;

	// Amount of time spent accelerating. Used to stop accelerating after target speed is reached, since speed cannot be accessed directly.
	float TimeAccelerated = 0.f;
	float TimeNeededToAccelerate = 0.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Accelerate to a target speed along its given walk spline in a set duration
	UFUNCTION(BlueprintCallable)
	void AccelerateToSpeed(float TargetSpeed, float Duration);

private:
	UFUNCTION()
	void OnLightDisablerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
