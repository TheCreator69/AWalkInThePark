

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineMovementComponent.generated.h"

class AWalkPath;

// Component the moves its owner along a spline path. This involves updating owner's location and rotation to match the spline's
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AWALKINTHEPARK_API USplineMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USplineMovementComponent();

	// The current path the owner is walking along
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<AWalkPath> CurrentPath;

	// Maximum speed the owner can reach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed = 125.f;

	// Maximum camera pitch offset (for looking up/down)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 90.f, ClampMax = 360.f))
	float MaxPitchOffset = 60.f;

	// Maximum camera yaw offset (for looking left/right)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 180.f, ClampMax = 360.f))
	float MaxYawOffset = 120.f;

private:
	// Current movement speed, duh
	float CurrentSpeed = 0.f;

	// How far the owner has moved from the origin of the spline along its path
	float DistanceAlongSpline = 0.f;

	// Update the distance based on owner's current movement speed
	void UpdateDistanceAlongSpline(float DeltaTime);

	// Set owner's transform based on its distance along the spline
	void SetOwnerTransformAlongSpline() const;

	/*
	* How much the camera's rotation should differ from the spline's at owner's current location.
	* Used for turning the camera while walking.
	*/
	FRotator CameraRotationOffset = FRotator(0.f, 0.f, 0.f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Add the specified amount to the current movement speed.
	* The amount is added in 1 second, so if the function is only called for 0.1 seconds, only 10% of the full amount is added.
	* That way, the caller can estimate how much speed is actually being added and the function is kept frame rate independent.
	*/ 
	void AddToMovementSpeed(float SpeedOffsetPerSecond);

	// Add a rotation offset to the player's camera. The yaw and pitch are capped based on the values provided to this component
	UFUNCTION(BlueprintCallable)
	void AddCameraRotationOffset(FRotator Offset);

	// Get the current camera rotation offset for the player's camera.
	FRotator GetCameraRotationOffset() const;

	// Set the rotation offset of the player's camera. The yaw and pitch are capped based on the values provided to this component
	void SetCameraRotationOffset(FRotator NewOffset);

	// Return how close the player is at their max speed, ranging from 0 (standing still) to 1 (at max speed)
	float GetPlayerSpeedPercentage() const;

	/*
	* Get the rotation of the currently active spline where the player is standing.
	* Doesn't take camera offset into account, so it only represents the player's control rotation if the player is facing straight ahead
	*/
	FRotator GetPlayerSplineRotation() const;
};
