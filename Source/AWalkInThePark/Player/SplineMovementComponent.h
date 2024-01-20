

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
	* Used for turning the camera while walking
	*/
	FRotator CameraRotationOffset = FRotator(0.f, 0.f, 0.f);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddToMovementSpeed(float SpeedOffsetPerSecond);
	void AddCameraRotationOffset(FRotator Offset);
};
