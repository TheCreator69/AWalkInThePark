

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineMovementComponent.generated.h"

class AWalkPath;
class UCameraShakeBase;

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

	// Camera shake used while player is standing still.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShakeBase> IdleCameraShake;

	// Camera shake used while player is walking slowly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShakeBase> SlowWalkCameraShake;

	// Camera shake used while player is walking quickly.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShakeBase> FastWalkCameraShake;

	// Maximum speed the owner can reach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed = 125.f;

	// Maximum camera pitch offset (for looking up/down)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 90.f, ClampMax = 360.f))
	float MaxPitchOffset = 60.f;

	// Maximum camera yaw offset (for looking left/right)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (UIMin = 0.f, ClampMin = 0.f, UIMax = 180.f, ClampMax = 360.f))
	float MaxYawOffset = 120.f;

	// Max overshoot allowed for camera pitch. The camera will allow this additional pitch but correct itself to MaxPitchOffset over time if left alone.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxPitchOvershoot = 5.f;

	// Max overshoot allowed for camera yaw. The camera will allow this additional yaw but correct itself to MaxYawOffset over time if left alone.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxYawOvershoot = 10.f;

	// I am a bit stupid, so this is a hack variable to let the component know it's supposed to respect the player's location and rotation offset while sitting.
	bool bSitMode = false;

	// To reposition the player to sit on the bench
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector SitModeLocation;

	// To let the component know which way is forward while the player is sitting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FRotator SitModeBaseOffset;

	// To rotate the owner's mesh if needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FRotator StandingMeshBaseOffset = FRotator(0, -90, 0);

	// Speed at which the camera rotation is supposed to be interpolated to the target rotation offset.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float RotationOffsetInterpSpeed = 20.f;

	// Speed at which camera overshoot should be corrected.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float OvershootCorrectionInterpSpeed = 3.f;

private:
	// Current movement speed, duh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CurrentSpeed = 0.f;

	// How far the owner has moved from the origin of the spline along its path
	float DistanceAlongSpline = 0.f;

	// Update the distance based on owner's current movement speed
	void UpdateDistanceAlongSpline(float DeltaTime);

	// Set owner's transform based on its distance along the spline
	void SetOwnerTransformAlongSpline() const;

	// Properly rotate the owner's skeletal mesh according to the current spline rotation
	void SetOwnerMeshRotation() const;

	// Stop owner when they reach the end of the current spline.
	void StopOwnerWhenEndReached();

	// Play the appropriate camera shake based on owner's speed. Used for player only.
	void PlayAppropriateCameraShake();

	/*
	* How much the camera's rotation should differ from the spline's at owner's current location.
	* Used for turning the camera while walking. This is the target location this component intends to reach.
	*/
	FRotator CameraRotationOffset = FRotator(0.0, 0.0, 0.0);

	// Correct camera overshoot for both pitch and yaw over time, if present.
	void CorrectCameraOvershoot();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Change the walk path to the specified one and position owner at the start.
	* Doesn't check whether or not the current walk path is the same as the newly specified one.
	*/
	void ChangePath(AWalkPath* NewPath);

	/*
	* Add the specified amount to the current movement speed.
	* The amount is added in 1 second, so if the function is only called for 0.1 seconds, only 10% of the full amount is added.
	* That way, the caller can estimate how much speed is actually being added and the function is kept frame rate independent.
	*/ 
	void AddToMovementSpeed(float SpeedOffsetPerSecond);

	// Reset movement speed, stopping owner
	void StopMovement();

	// Add a rotation offset to the player's camera. The yaw and pitch are capped based on the values provided to this component
	UFUNCTION(BlueprintCallable)
	void AddCameraRotationOffset(FRotator Offset);

	// Get the current camera rotation offset for the player's camera.
	FRotator GetCameraRotationOffset() const;

	// Set the rotation offset of the player's camera. The yaw and pitch are capped based on the values provided to this component
	void SetCameraRotationOffset(FRotator NewOffset);

	// Return how close the player is at their max speed, ranging from 0 (standing still) to 1 (at max speed)
	UFUNCTION(BlueprintCallable)
	float GetPlayerSpeedPercentage() const;

	/*
	* Get the rotation of the currently active spline where the player is standing.
	* Doesn't take camera offset into account, so it only represents the player's control rotation if the player is facing straight ahead
	*/
	FRotator GetPlayerSplineRotation() const;
};
